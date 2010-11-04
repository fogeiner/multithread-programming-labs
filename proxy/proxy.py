#! /usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function
import cfg
import socket
import logging as l
import select
import re
from urlparse import urlparse
import Queue

LOG_FILENAME = 'proxy.log'
LEVELS = {'debug': l.DEBUG,
          'info': l.INFO,
          'warning': l.WARNING,
          'error': l.ERROR,
          'critical': l.CRITICAL}

l.basicConfig(level=l.DEBUG)

class TaskDispatcher(Queue.Queue):
	def run(self):
		while True:	
			task = self.get()
			l.debug('processing new task')
			task.run(self)

class Task:
	def run(self, task_dispatcher):
		pass

class StartServerTask(Task):
	def run(self, task_dispatcher):
		l.debug('starting server')
		Proxy.l_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		Proxy.l_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		l.debug('listening socket {0}'.format(Proxy.l_sock.fileno()))
		Proxy.l_sock.bind(('', Proxy.l_port))
		l.debug('listening socket bound to {0}'.format(Proxy.l_port))
		Proxy.l_sock.listen(Proxy.backlog)
		l.debug('putting SelectTask')
		task_dispatcher.put(SelectTask())

class ProxyClient:
	_request = ""
	_request_read = False
	def __init__(self, sock, addr):
		l.debug('creating ProxyClient with ' + repr(addr) + ' socket ' + str(sock.fileno()))
		self._socket = sock
		self._addr = addr
	def fileno(self, *args):
		return self._socket.fileno()

class ProcessClientRequestTask(Task):
	def __init__(self, proxy_client):
		self._proxy_client = proxy_client
	def run(self, task_dispatcher):
		# we support only GET and HEAD queries and HTTP/1.0 only
		# we need server DNS/IP and path on server
		request = self._proxy_client._request

		m = re.match(r'^(GET|HEAD)\s+(.+)\s+HTTP/(1.\d)$', request, re.M)

		if m:
			method, url, version = m.groups()
			parsed_url = urlparse(url)

			if (method not in Proxy.supported_methods
					or version not in Proxy.supported_http_versions
					or parsed_url.scheme != 'http'):
				l.debug('bad request from the client')
				# here we need to send client msg about the bad request
				pass

			# got some valid input
		else:
			# got some junk, just ingoring
			pass

class ReadClientTask(Task):
	def __init__(self, proxy_client):
		self._proxy_client = proxy_client

	def run(self, task_dispatcher):
		buf = self._proxy_client._socket.recv(Proxy.recv_buf_size)
		self._proxy_client._request += buf

		if buf == '':
			l.debug('client closed connection')
			self._proxy_client._request_read = True

		if self._proxy_client._request[-4:] == "\r\n\r\n":
			l.debug('putting ProcessClientRequestTask')
			l.debug('client request: ' + self._proxy_client._request)
			self._proxy_client._request_read = True
			task_dispatcher.put(ProcessClientRequestTask(self._proxy_client))

class SelectTask(Task):
	def run(self, task_dispatcher):
		rlist, wlist, xlist = [Proxy.l_sock], [], []
		# adding potential sockets
		
		# not all IRL
		rlist += [i for i in Proxy.clients if i._request_read == False]

		rlist, wlist, xlist = select.select(rlist, wlist, xlist)
		
		if Proxy.l_sock in rlist:
			l.debug('putting AcceptClientTask')
			task_dispatcher.put(AcceptClientTask())

		for sock in rlist:
			# a better idea to disting. server and client sockets
			# would be a polymorphism but not for now
			if sock in Proxy.clients:
				l.debug('putting ReadClientTask')
				task_dispatcher.put(ReadClientTask(sock))

			# putting other tasks to do with reading
		
		for sock in wlist:
			pass
			# putting taks to do with writing

		l.debug('putting SelectTask')
		task_dispatcher.put(SelectTask())



class AcceptClientTask(Task):
	def run(self, task_dispatcher):
		c_sock, c_addr = Proxy.l_sock.accept()
		Proxy.clients.append(ProxyClient(c_sock, c_addr))

class Proxy:
	task_dispatcher = TaskDispatcher()
	backlog = 5
	l_sock = None
	l_port = 8080
	recv_buf_size = 1024
	clients = []
	supported_methods = ['HEAD', 'GET']
	supported_http_versions = ['1.0']
	def run(self):
		l.debug('put StartServerTask')
		Proxy.task_dispatcher.put(StartServerTask())
		l.debug('run task_dispatcher')
		Proxy.task_dispatcher.run()

if __name__ == '__main__':
	proxy = Proxy()
	proxy.run()
