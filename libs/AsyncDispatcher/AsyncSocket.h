#pragma once
#include "../TCPSocket/TCPSocket.h"
#include "Async.h"

class AsyncSocket: public TCPSocket, public Async {
	public:
		AsyncSocket(): TCPSocket(){
		}	
		AsyncSocket(int sock, struct sockaddr *addr): TCPSocket(sock, addr) {
		}
		AsyncSocket(const AsyncSocket &orig): TCPSocket(orig) {
		}
		AsyncSocket& operator=(const AsyncSocket &orig){
			TCPSocket::operator=(orig);
		}
		~AsyncSocket(){
		}
};


