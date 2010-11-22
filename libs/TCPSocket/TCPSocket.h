#pragma once
#include <list>
#include <vector>
#include <exception>
#include <string>

#include <cerrno>
#include <cstring>
#include <cassert>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "../Fd_set/Fd_set.h"
#include "../Buffer/Buffer.h"
#include "Selectable.h"

#ifdef DEBUG
#include <cstdio>
#endif

std::vector<std::list<Selectable*> > 
Select(std::list<Selectable*> rlist,
		std::list<Selectable*> wlist,
		std::list<Selectable*> xlist){
	Fd_set rfds, wfds, xfds;

	std::vector<std::list<Selectable*> > lists;
	return lists;
}

class TCPSocketException: public std::exception {
	private:
		std::string _err;
		static const int ERR_MSG_MAX_LENGTH = 256;
	public:
		TCPSocketException(const char *msg): _err(msg) {

		}

		TCPSocketException(int err_number){
			char buf[ERR_MSG_MAX_LENGTH];
			char *msg_ptr;
			msg_ptr = ::strerror_r(err_number, buf, sizeof(buf));
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE
			this->_err.assign(buf);
#else
			this->_err.assign(msg_ptr);	
#endif
		}

		const char *what() const throw(){
			return this->_err.c_str();
		}

		~TCPSocketException() throw() {}
};

class SocketException: public TCPSocketException {
	public:
		SocketException(const char *msg): TCPSocketException(msg){
		}

		SocketException(int err_number): TCPSocketException(err_number){
		}
};

class SockOptException: public TCPSocketException {
	public:
		SockOptException(const char *msg): TCPSocketException(msg){
		}

		SockOptException(int err_number): TCPSocketException(err_number){
		}
};

class ConnectException: public TCPSocketException {
	public:
		ConnectException(const char *msg): TCPSocketException(msg){
		}

		ConnectException(int err_number): TCPSocketException(err_number){
		}
};

class AcceptException: public TCPSocketException {
	public:
		AcceptException(const char *msg): TCPSocketException(msg){
		}

		AcceptException(int err_number): TCPSocketException(err_number){
		}
};


class IOException: public TCPSocketException {
	public:
		IOException(const char *msg): TCPSocketException(msg){
		}

		IOException(int err_number): TCPSocketException(err_number){
		}
};

class RecvException: public IOException {
	public:
		RecvException(const char *msg): IOException(msg){
		}

		RecvException(int err_number): IOException(err_number){
		}
};


class SendException: public IOException {
	public:
		SendException(const char *msg): IOException(msg){
		}

		SendException(int err_number): IOException(err_number){
		}
};

class BindException: public TCPSocketException {
	public:
		BindException(const char *msg): TCPSocketException(msg){
		}

		BindException(int err_number): TCPSocketException(err_number){
		}
};

class ListenException: public TCPSocketException {
	public:
		ListenException(const char *msg): TCPSocketException(msg){
		}

		ListenException(int err_number): TCPSocketException(err_number){
		}
};

class DNSException: public TCPSocketException {
	public:
		DNSException(const char *msg): TCPSocketException(msg){
		}

		DNSException(int err_number): TCPSocketException(hstrerror(err_number)) {
		}
};

class TCPSocket: public Selectable {
	private:
		struct Base {
			int _sock;
			int _links;

			Base(int sock, int links = 1): _sock(sock), _links(links) {
			}
		};


		void increase() {
			assert(_b->_links != 0);
			_b->_links++;
#ifdef DEBUG
			fprintf(stderr, "socket %d increase: now %d\n",_b->_sock, _b->_links);
#endif
		}

		void decrease() {
			assert(_b->_links > 0);

			_b->_links--;

#ifdef DEBUG
			fprintf(stderr, "socket %d decrease: now %d\n", _b->_sock, _b->_links);
#endif
			if (_b->_links == 0) {
				this->close();
			}
		}

		enum TCPSocketState {ACTIVE, CLOSED};
		TCPSocketState _state;
		TCPSocket::Base *_b;
		struct sockaddr _addr;
	public:

		TCPSocket() {
			int sock = ::socket(AF_INET, SOCK_STREAM, 0);
			if(sock == -1){
				throw SocketException(errno);
			}

			this->_b = new TCPSocket::Base(sock);
			this->_state = ACTIVE;
#ifdef DEBUG
			fprintf(stderr, "created socket %d\n", _b->_sock);
#endif
		}

		TCPSocket(int sock, struct sockaddr addr){
			this->_b = new TCPSocket::Base(sock);
			this->_addr = addr;
#ifdef DEBUG
			fprintf(stderr, "created socket %d\n", _b->_sock);
#endif
			this->_state = ACTIVE;
		}

		TCPSocket(const TCPSocket &orig) {
			this->_b = orig._b;
			this->increase();
		}

		~TCPSocket() {
			this->decrease();
		}

		TCPSocket& operator=(const TCPSocket &orig) {
			if (this == &orig) {
				return *this;
			}

			this->decrease();
			this->_b = orig._b;
			this->increase();
			return *this;
		}

		int fileno() {
			return this->_b->_sock;
		}

		void getsockopt(int level, int optname, void *optval, socklen_t *optlen) {
			if(::getsockopt(this->_b->_sock, level, optname, optval, optlen) == -1){
				throw SockOptException(errno);
			}
		}

		void setsockopt(int level, int optname, const void *optval, socklen_t optlen) {

			if(::setsockopt(this->_b->_sock, level, optname, optval, optlen) == -1){
				throw SockOptException(errno);
			}
		}

		void set_reuse_addr(int value) {
			this->setsockopt(SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
		}

		void listen(int backlog){
			if(::listen(this->_b->_sock, backlog) == -1){
				throw ListenException(errno);
			}
		}

		void close() {
			if(this->_state != CLOSED){
#ifdef DEBUG
				fprintf(stderr, "socket %d closing\n", _b->_sock);
#endif
				::close(_b->_sock);
				delete this->_b;
				this->_state = CLOSED;
			}
		}

		int recv(Buffer &b, int count = 4096){
			return this->recv(&b, count);
		}

		int recv(Buffer *b, int count = 4096) {
			assert(count > 0);
			char buf[count];

			int read = ::recv(this->_b->_sock, buf, sizeof(buf), MSG_NOSIGNAL);
#ifdef DEBUG
			fprintf(stderr, "socket %d recv %d bytes\n", _b->_sock, read);
#endif

			if(read == -1){
				throw RecvException(errno);
			}

			if(read > 0) {
				b->append(buf, read);
			}


			return read;
		}

		int send(Buffer &buf) {
			return this->send(&buf);
		}

		int send(Buffer &buf, int count) {
			return this->send(&buf, count);
		}

		int send(Buffer *buf) {
			return this->send(buf, buf->size());
		}
		int send(Buffer *buf, int count) {
			assert(buf->size() >= count);

			int sent = ::send(this->_b->_sock, buf->buf(), count, MSG_NOSIGNAL);
#ifdef DEBUG
			fprintf(stderr, "socket %d sent %d bytes\n", _b->_sock, sent);
#endif

			if(sent == -1){
				throw SendException(errno);
			}

			return sent;
		}

		void bind(unsigned short port) {

			struct sockaddr_in local_addr;
			memset(&local_addr, 0, sizeof(local_addr));
			local_addr.sin_family = AF_INET;
			local_addr.sin_port = htons(port);
			local_addr.sin_addr.s_addr = INADDR_ANY;

			if(::bind(this->_b->_sock, (const sockaddr*)&local_addr, sizeof(local_addr)) == -1) {
				throw BindException(errno);
			}

#ifdef DEBUG
			fprintf(stderr, "socket %d bound to port %d\n", _b->_sock, port);
#endif
		}

		void connect(const char *name, unsigned short port) {
			struct sockaddr_in remote_addr;

			const int GETHOSTBYNAME_R_BUFSIZE = 256;
			char tmp_buf[GETHOSTBYNAME_R_BUFSIZE];
			struct hostent ret, *result;
			int h_errnop;
			if(gethostbyname_r(name, &ret, tmp_buf, sizeof(tmp_buf),
						&result, &h_errnop) == -1){
				throw DNSException(h_errno);
			}

			if (result == NULL){
				throw DNSException(h_errno);
			}

			memset(&remote_addr, 0, sizeof(remote_addr));
			remote_addr.sin_addr = *((in_addr *) result->h_addr_list[0]);
			remote_addr.sin_family = AF_INET;
			remote_addr.sin_port = htons(port);

			if(::connect(this->_b->_sock, (struct sockaddr*)&remote_addr, sizeof(remote_addr)) == -1){
				throw ConnectException(errno);
			}
		}

		void connect(std::string &name, short port) {
			return this->connect(name.c_str(), port);
		}

		TCPSocket accept(){
			struct sockaddr addr;
			socklen_t len = sizeof(addr);
			int n_sock = ::accept(this->_b->_sock, &addr, &len);
#ifdef DEBUG
			fprintf(stderr, "socket %d accepted %d\n", this->_b->_sock, n_sock);
#endif
			if(n_sock == -1){
				throw AcceptException(errno);
			}
			return TCPSocket(n_sock, addr);
		}
};
