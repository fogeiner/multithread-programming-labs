import config 
from ProxyConnection import ProxyClient
from Logger import *

class Retranslator(object):
    def __init__(self, server, clients, header_end_index, buffer):
        l.debug('Created Retranslator for ' + str(len(clients)) + ' clients')
        self._server = server
        self._clients = clients
        self._header_end_index = header_end_index
        for c in clients:
            c._retranslator = self
            c._status = ProxyClient.RETRANSLATOR
            if c._method == 'HEAD':
                c._send_buf += buffer[:header_end_index + len(config.http_delimeter)]
            if c._method == 'GET':
                c._send_buf += buffer
    def fileno(self):
        return self._server.fileno()