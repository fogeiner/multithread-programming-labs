#! /usr/bin/env python

import socket as s

buf_size = 1024
port = 2222

def process_client(c_sock):
	buf = ''
	while True:	
		buf += c_sock.recv(buf_size)
		print(buf,)
		if buf[-4:] == '\r\n\r\n':
			break
	c_sock.send("HTTP/1.0 505 Protocol version not supported\r\n\r\nNo HTTP/1.1")
	c_sock.close()

if __name__ == '__main__':
	l_sock = s.socket(s.AF_INET, s.SOCK_STREAM)
	l_sock.bind(('', port))
	l_sock.listen(5)

	while True:
		c_sock, c_addr = l_sock.accept()
		print "Accepted connection:\n" + "socket: \t" + repr(c_sock) + "addr: \n" + repr(c_addr)
		process_client(c_sock)

