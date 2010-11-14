from Retranslator import Retranslator
from ProxyConnection import ProxyClient, ProxyHTTPServer, Connection
from Cache import Cache, CacheEntry
import socket
import signal
import select
import re
from urlparse import urlparse

from Logger import *
from TaskDispatcher import TaskDispatcher
import config

class Task(object):
    """ 'abstract' class representing a Task """
    def __init__(self):
        pass
    def run(self):
        pass

class AcceptClientTask(Task):
    PRIORITY = 0
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        l.debug('AcceptClientTask run')
        c_sock, c_addr = self._proxy._l_sock.accept()
        l.debug('New client; socket: ' + repr(c_sock) + ' addr: ' + repr(c_addr))
        client = ProxyClient(c_sock, c_addr)
        client._status = ProxyClient.GETTING_REQUEST
        self._proxy._new_clients.append(client)

class ReceiveRequestTask(Task):
    PRIORITY = 0
    def __init__(self, proxy, client):
        self._proxy = proxy
        # @type self._client ProxyClient
        self._client = client
    def run(self):
        l.debug('ReceiveRequestTask run')
        request_fr = self._client._c_sock.recv(self._proxy._recv_bufsize)

        # unexpectedly closed connection
        if request_fr == '':
            l.debug('client unexpectedly closed connection')
            self._client.close()
            self._proxy._new_clients.remove(self._client)

        self._client._request += request_fr

        # end of request
        if self._client._request[-4:] == config.http_delimeter:
            self._client._status = ProxyClient.PARSING_REQUEST
            self._proxy._t_d.put( (ParseRequestTask.PRIORITY, ParseRequestTask(self._proxy, self._client)) )
    
class ParseRequestTask(Task):
    PRIORITY = 0
    def __init__(self, proxy, client):
        self._proxy = proxy
        # @type self._client ProxyClient
        self._client = client

    def _bad_request(self):
        l.debug('Bad request from the client')
        self._client.close()
        self._proxy._new_clients.remove(self._client)

    def run(self):
        l.debug('ParseRequestTask run')
    
        m = re.match(r'^(\w+)\s+(.+)\s+HTTP/(1.\d)\s+$', self._client._request, re.M)
        
        if m:
            method, url, version = m.groups()
            l.debug(method + ' ' + url + ' ' + version)
            parsed_url = urlparse(url)

            if (method in self._proxy._supported_http_methods and parsed_url.scheme == 'http'):
                self._client._parsed_url = parsed_url
                self._client._url = url
                self._client._method = method
                self._proxy._t_d.put(
                    (ProcessRequestTask.PRIORITY, ProcessRequestTask(self._proxy, self._client)) )
            else:
                self._bad_request()
        else:
            self._bad_request()
            
class ProcessRequestTask(Task):
    PRIORITY = 0
    def __init__(self, proxy, client):
        self._proxy = proxy
        self._client = client
    def run(self):
        l.debug('ProcessRequestTask run')
        cache = self._proxy._cache
        cache.lock()

        cache_entry = cache.get(self._client._url)
        # if the url is present in the cache
        if cache_entry != None:
            l.debug('CacheEntry found')
            cache_entry.lock()
            cache_entry.add_client(self._client)
            cache_entry.unlock()
        # if not present
        else:
            l.debug('creating new CacheEntry')
            # create new entry and add as client
            entry = CacheEntry(self._client._url, self._client._parsed_url, self._client._request)
            entry.add_client(self._client)
            cache.add_entry(self._client._url, entry)

            self._proxy._t_d.put( (InitServerConnectionTask.PRIORITY, InitServerConnectionTask(self._proxy, entry)) )

        cache.unlock()


class InitServerConnectionTask(Task):
    PRIORITY = 0
    def __init__(self, proxy, cache_entry):
        self._proxy = proxy
        self._cache_entry = cache_entry
    def run(self):
        l.debug('InitServerConnectionTask run')
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            netloc = self._cache_entry._parsed_url.netloc
            port = self._cache_entry._parsed_url.port
            if port == None: port = self._proxy._default_http_port
            s.connect((netloc, port))
            self._cache_entry._server = ProxyHTTPServer(s)
        except Exception as ex:
            l.error('InitServerConnectionTask ' + repr(ex))
            


class SendRequestTask(Task):
    PRIORITY = 0
    def __init__(self, proxy, cache_entry):
        self._proxy = proxy
        self._cache_entry = cache_entry
    def run(self):
        l.debug('SendRequestTask run')
        entry = self._cache_entry
        entry._server.send(entry._request[entry._server._bytes_sent:])
        if entry._server._bytes_sent == len(entry._request):
            l.debug('Request fully sent')
            entry._server._status = ProxyHTTPServer.GETTING_RESPONSE


class ReceiveResponseTask(Task):
    PRIORITY = 0
    def __init__(self, proxy, cache_entry):
        self._proxy = proxy
        self._cache_entry = cache_entry
    def run(self):
        l.debug('ReceiveResponseTask run')
        entry = self._cache_entry
        entry._buf += entry._server.recv(self._proxy._recv_bufsize)
        header_end_index = entry._buf.find(config.http_delimeter)
        if header_end_index != -1:
            entry._header = entry._buf[:header_end_index]
            entry._buf = entry._buf[header_end_index+len(config.http_delimeter):]
            entry._server._status = ProxyHTTPServer.PARSING_RESPONSE
            self._proxy._t_d.put( (ParseResponseTask.PRIORITY, ParseResponseTask(self._proxy, entry)) )

class ParseResponseTask(Task):
    PRIORITY = 0
    def __init__(self, proxy, cache_entry):
        self._proxy = proxy
        self._cache_entry = cache_entry
    def run(self):
        l.debug('ParseResponseTask run')
        entry = self._cache_entry
        m = re.match(r'^HTTP/(1\.\d)\s+(\d{3})\s+(.*)$', entry._header, re.M)
        if m:
            version, code, comment = m.groups()
            l.debug(version +' '+ code + ' ' + comment)

            if code == self._proxy._http_OK_code:
                # caching and sending to clients
                entry._server._status = ProxyHTTPServer.CACHE
            else:
                # retranslating
                entry._server._status = ProxyHTTPServer.RETRANSLATOR
                # get the entry out of Cache
                self._proxy._cache.remove_by_key(entry._url)
                # add to list of retranslated connections
                retranslator = Retranslator(entry._server, entry._clients, entry._header, entry._buf)
                self._proxy._retranslated_connections.append(retranslator)
        else:
            l.debug('Server sent incorrect reply')
            pass

class SendClientDataTask(Task):
    def __init__(self, proxy, entry):
        self._proxy = proxy
    def run(self):
        pass


class ReceiveServerDataTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        pass

class RetranslatorReceiveTask(Task):
    PRIORITY = 0
    def __init__(self, proxy, retranslator):
        self._proxy = proxy
        self._retranslator = retranslator
    def run(self):
        l.debug('RetranslatorReceiveTask run')
        retranslator = self._retranslator
        proxy = self._proxy
        
        buf = retranslator._server.recv(proxy._recv_bufsize)
        # end of transmission
        if buf == '':
            retranslator._server.close()
            retranslator._server = Connection.CLOSED_CONNECTION
        else:
            for c in retranslator._clients:
                if c._method != 'HEAD':
                    c._send_buf += buf

class RetranslatorSendTask(Task):
    PRIORITY = 0
    def __init__(self, proxy, client):
        self._proxy = proxy
        self._client = client
    def run(self):
        l.debug('RetranslatorSendTask run')
        retranslator = self._client._retranslator
        client = self._client
        
        sent = client.send(client._send_buf)
        client._send_buf = client._send_buf[sent:]
        
        if len(client._send_buf) == 0 and retranslator._server == Connection.CLOSED_CONNECTION:
            client.close()
            client._c_sock = Connection.CLOSED_CONNECTION
            retranslator._clients.remove(client)
            if len(retranslator._clients) == 0:
                self._proxy._retranslated_connections.remove(retranslator)


class SelectTask(Task):
    PRIORITY = 20
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        l.debug('SelectTask run')
        proxy = self._proxy
        cache = proxy._cache

        rlist, wlist, xlist = [], [], []
        # read
        # listening socket
        rlist.append(proxy._l_sock)

        # clients that haven't sent the request yet
        rlist += [s for s in proxy._new_clients if s._status == ProxyClient.GETTING_REQUEST]

        # servers in Cache
        for e in cache.entries():
            if e._server._status == ProxyHTTPServer.GETTING_RESPONSE:
                rlist.append(e)

        # servers in Retranslator
        for r in proxy._retranslated_connections:
            if r._server != Connection.CLOSED_CONNECTION:
                rlist.append(r)
        # write
        # writing request to servers
        for e in cache.entries():
            if e._server._status == ProxyHTTPServer.SENDING_REQUEST:
                wlist.append(e)
        # writing to clients in retranslator
        for r in proxy._retranslated_connections:
            for c in r._clients:
                if len(c._send_buf) != 0:
                    wlist.append(c)

        rlist, wlist, xlist = select.select(rlist, wlist, xlist)
        import time
        time.sleep(1)

        l.debug('Select results: ' + repr(rlist) +' '+ repr(wlist) +' '+ repr(xlist))

        for s in rlist:
            # new client?
            if s == self._proxy._l_sock:
                proxy._t_d.put((AcceptClientTask.PRIORITY, AcceptClientTask(proxy)))
            elif s in proxy._retranslated_connections:
                proxy._t_d.put( (RetranslatorReceiveTask.PRIORITY, RetranslatorReceiveTask(proxy, s)) )
            elif s._status == ProxyClient.GETTING_REQUEST:
                proxy._t_d.put((ReceiveRequestTask.PRIORITY, ReceiveRequestTask(proxy, s)))
            elif s._server._status == ProxyHTTPServer.GETTING_RESPONSE:
                proxy._t_d.put( (ReceiveResponseTask.PRIORITY, ReceiveResponseTask(proxy, s)) )
                
        for s in wlist:
            if s.__class__ == CacheEntry:
                if s._server._status == ProxyHTTPServer.SENDING_REQUEST:
                    # @type s CacheEntry
                    proxy._t_d.put( (SendRequestTask.PRIORITY, SendRequestTask(proxy, s)) )
            if s.__class__ == ProxyClient:
                proxy._t_d.put( (RetranslatorSendTask.PRIORITY, RetranslatorSendTask(proxy, s)) )


        # planning next SelectTask
        proxy._t_d.put( (SelectTask.PRIORITY, SelectTask(proxy)) )
    

class StopServerTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        pass

class StartServerTask(Task):
    PRIORITY = 0
    """ starts proxy server """
    def __init__(self, proxy):
        self._proxy = proxy

    def run(self):
        l.debug('StartServerTask run')
        l_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        l_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        l_socket.bind(('', self._proxy._listening_port))
        l_socket.listen(self._proxy._backlog)

        self._proxy._l_sock = l_socket

        self._proxy._t_d.put( (SelectTask.PRIORITY, SelectTask(self._proxy)) )
            
class Proxy(object):
    def __init__(self):
        self._t_d = TaskDispatcher()
        self._backlog = config.backlog
        self._listening_port = config.listening_port
        self._recv_bufsize = config.recv_bufsize
        self._send_bufsize = config.send_bufsize
        self._max_cache_entry_size = config.max_cache_entry_size
        self._supported_http_methods = config.supported_http_methods
        self._default_http_port = config.default_http_port
        self._listening_socket = None
        self._cache = Cache()
        self._retranslated_connections = []
        self._http_OK_code = config.http_OK_code

        # clients that connected but haven't sent the request yet
        # list of ProxyClients
        self._new_clients = []
    def start(self):
        start_server_task = StartServerTask(self)
        self._t_d.put( (StartServerTask.PRIORITY, start_server_task) )
        # in case we use threads here should be threads creation
        l.debug('running task_dispatcher')
        self._t_d.run()

def sigint_handler(signum, frame):
    proxy._t_d.put(StopServerTask(proxy))

proxy = None

if __name__ == "__main__":
    proxy = Proxy()
    signal.signal(signal.SIGINT, sigint_handler)
    proxy.start()
