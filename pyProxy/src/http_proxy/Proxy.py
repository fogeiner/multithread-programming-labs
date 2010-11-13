from Retranslator import Retranslator
from ProxyConnection import ProxyClient, ProxyHTTPServer
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
        client._type = ProxyClient.GETTING_REQUEST
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
        if self._client._request[-4:] == '\r\n\r\n':
            self._client._type = ProxyClient.PARSING_REQUEST
            self._proxy._t_d.put(ParseRequestTask(self._proxy, self._client))
    
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
    
        m = re.match(r'^(GET|HEAD)\s+(.+)\s+HTTP/(1.\d)\s+$', self._client._request, re.M)
        
        if m:
            method, url, version = m.groups()
            l.debug(method + ' ' + url + ' ' + version)
            parsed_url = urlparse(url)

            if (method in self._proxy._supported_http_methods and parsed_url.scheme == 'http'):
                self._client._parsed_url = parsed_url
                self._client._url = url
                self._proxy._t_d.put(
                    ProcessRequestTask(self._proxy, self._client), ProcessRequestTask.PRIORITY)
            else:
                self._bad_request()
        else:
            self._bad_request()
            
class ProcessRequestTask(Task):
    PRIORITY = 5
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
            entry = CacheEntry(self._client._parsed_url, self._client._request)
            entry.add_client(self._client)
            cache.add_entry(self._client._url, entry)

        cache.unlock()


class InitServerConnectionTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        pass


class SendRequestTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        pass

class ReceiveResponseTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        pass

class ParseResponseTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        pass

class SendClientDataTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        pass


class ReceiveServerDataTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        pass

class SelectTask(Task):
    PRIORITY = 20
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        l.debug('SelectTask run')

        
        rlist, wlist, xlist = [], [], []
        # read
        # listening socket
        rlist.append(self._proxy._l_sock)

        # clients that haven't sent the request yet
        rlist += [s for s in self._proxy._new_clients if s._type == ProxyClient.GETTING_REQUEST]

        # clients in Cache
        # clients in Retranslator


        
        rlist, wlist, xlist = select.select(rlist, wlist, xlist)

        for s in rlist:
            # new client?
            if s == self._proxy._l_sock:
                self._proxy._t_d.put(AcceptClientTask(self._proxy))
            elif s._type == ProxyClient.GETTING_REQUEST:
                self._proxy._t_d.put(ReceiveRequestTask(self._proxy, s))
            elif s._type == ProxyClient.RETRANSLATOR:
                pass
            elif s._type == ProxyClient.CACHE:
                pass

        # planning next SelectTask
        self._proxy._t_d.put(SelectTask(self._proxy))
    

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

        select_task = SelectTask(self._proxy)
        self._proxy._t_d.put(select_task, SelectTask.PRIORITY)
            
class Proxy(object):
    def __init__(self):
        self._t_d = TaskDispatcher()
        self._backlog = config.backlog
        self._listening_port = config.listening_port
        self._recv_bufsize = config.recv_bufsize
        self._send_bufsize = config.send_bufsize
        self._max_cache_entry_size = config.max_cache_entry_size
        self._supported_http_methods = config.supported_http_methods
        self._listening_socket = None
        self._cache = Cache()
        self._retranslator = Retranslator()

        # clients that connected but haven't sent the request yet
        # list of ProxyClients
        self._new_clients = []
    def start(self):
        start_server_task = StartServerTask(self)
        self._t_d.put(start_server_task, StartServerTask.PRIORITY)
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
