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
    pass

class ProxyHTTPServer(Connection):
    """ represents server side of the proxy connection """
    pass