from http_proxy.Retranslator import Retranslator
import socket
import signal

from Logger import *
from TaskDispatcher import TaskDispatcher
import config

class Task:
    """ 'abstract' class representing a Task """
    def __init__(self):
        self._done = True
    def run(self):
        pass

class AcceptClientTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        pass

class RecvRequestTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        pass
    
class ParseRequestTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        pass

class ProcessRequestTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        pass

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

class RecveiveResponseTask(Task):
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


class RecvServerDataTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        pass

class SelectTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        rlist, wlist, xlist = [self._proxy._l_sock], [], []

        rlist, wlist, xlist = select.select(rlist, wlist, xlist)

        if self._proxy._l_sock in rlist:
            _proxy._t_d.put(AcceptClientTask(self._proxy))

        self._proxy._t_d.put(SelectTask(self._proxy))

class StopServerTask(Task):
    def __init__(self, proxy):
        self._proxy = proxy
    def run(self):
        pass

class StartServerTask(Task):
    """ starts proxy server """
    def __init__(self, proxy):
        self._proxy = proxy

    def run(self):
        l.debug('StartServerTask run')
        l_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        l_socket.bind(('', self._proxy._listening_port))
        l_socket.listen(self._proxy._backlog)

        self._proxy._l_sock = l_socket

        select_task = SelectTask(self._proxy)
        self._proxy._t_d.put(select_task)
            
class Proxy:
    def __init__(self):
        self._backlog = config.backlog
        self._listening_port = config.listening_port
        self._recv_bufsize = config.recv_bufsize
        self._send_bufsize = config.send_bufsize
        self._max_cache_entry_size = config.max_cache_entry_size
        self._supported_http_methods = config.supported_http_methods
        self._listening_socket = None
        self._cache = Cache()
        self._retranslator = Retranslator()
        
    def start(self):
        self._t_d = TaskDispatcher()
        start_server_task = StartServerTask(task_dispatcher, self)
        self._t_d.put(start_server_task)
        # in case we use threads here should be threads creation
        l.debug('running task_dispatcher')
        task_dispatcher.run()

proxy = None

def sigint_handler(signum, frame):
    proxy._t_d.put(StopServerTask(proxy))

if __name__ == "__main__":
    proxy = Proxy()
    signal.signal(signal.SIGINT, sigint_handler)
    proxy.start()
