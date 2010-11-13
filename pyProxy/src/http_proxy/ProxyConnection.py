import time

__author__="alstein"
__date__ ="$11.11.2010 15:00:59$"

class Connection:
    """ represents abstract class for network connections """
    def __init__(self, socket, time=int(time.time())):
        self._last_upd_time = time
        self._sock = socket
    

class ProxyClient(Connection):
    """ represents client side of the proxy connection """
    # client was just created
    INIT = 0
    # client is in process of getting request
    GETTING_REQUEST = 1
    # client is in process of parsing request
    PARSING_REQUEST = 2
    # client is used in retranslator
    RETRANSLATOR = 3
    # client is used in cache transmission
    CACHE = 4

    def __init__(self, c_sock, c_addr):
        self._c_sock = c_sock
        self._c_addr = c_addr
        self._request = ''
        self._bytes_received = 0
        # what kind of connection is this client?
        self._type = ProxyClient.INIT

    def close(self):
        self._c_sock.close()

    def fileno(self):
        return self._c_sock.fileno()
    

class ProxyHTTPServer(Connection):
    """ represents server side of the proxy connection """
    # server was just created
    INIT = 0
    # server is in process of sending request
    GETTING_REQUEST = 1
    # server is used in retranslator
    RETRANSLATOR = 2
    # server is used in cache transmission
    CACHE = 3
    
    def __init__(self):
        pass
    def close(self):
        pass
    def fileno(self):
        pass