import config 
from ProxyConnection import ProxyClient
from Logger import *

class Retranslator(object):
    def __init__(self, server, clients, header, buffer):
        l.debug('Created Retranslator for ' + str(len(clients)) + ' clients')
        self._server = server
        self._clients = clients
        for c in clients:
            c._retranslator = self
            c._status = ProxyClient.RETRANSLATOR
            c._send_buf += header
            if c._method == 'GET':
                c._send_buf += config.http_delimeter
                c._send_buf += buffer
    def fileno(self):
        return self._server.fileno()