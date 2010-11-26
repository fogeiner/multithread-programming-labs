#pragma once
#define DEBUG
#include "../TCPSocket/TCPSocket.h"
#include "../TCPSocket/Selectable.h"
#include <list>

#ifdef DEBUG
#include <cstdio>
#endif

class AsyncSocket: public Selectable {
	public:
		virtual bool readable() = 0;
		virtual bool writable() = 0;
		virtual void handle_read() = 0;
		virtual void handle_write() = 0;
		virtual void handle_close() = 0;
		virtual void handle_accept() = 0;
		virtual ~AsyncSocket();
};

class AsyncDispatcher: public Selectable {
	private:
		static std::list<AsyncSocket*> _sockets;
		TCPSocket *_s;
	public:
		AsyncDispatcher(){
		}
		~AsyncDispatcher(){
		}
		AsyncDispatcher(const AsyncDispatcher &orig){
		}
		AsyncDispatcher &operator=(const AsyncDispatcher &orig){
		}

		static void loop(int timeout_ms = 0){
			std::list<Selectable*> rlist, wlist, xlist;
			for(std::list<AsyncSocket*>::iterator i = _sockets.begin();
					i != _sockets.end(); ++i){
				AsyncSocket *s = *i;
				if (s->readable()){
					rlist.push_back(s);
				}
				if (s->writable()){
					wlist.push_back(s);
				}
				if (s->readable() || s->writable()){
					xlist.push_back(s);
				}
			}

			Select(&rlist, &wlist, &xlist, timeout_ms);

			for(std::list<Selectable*>::iterator i = rlist.begin();
					i != rlist.end(); ++i){
				AsyncSocket* s = static_cast<AsyncSocket*>(*i);
				s -> handle_read();
			}

			for(std::list<Selectable*>::iterator i = wlist.begin();
					i != wlist.end(); ++i){
				AsyncSocket* s = static_cast<AsyncSocket*>(*i);
				s -> handle_write();
			}
		}

		virtual bool readable() {
#ifdef DEBUG
			fprintf(stderr, "unhandled readable()\n");
#endif
			return true;
		}
		virtual bool writable(){
#ifdef DEBUG
			fprintf(stderr, "unhandled writable()\n");
#endif
			return true;
		}

		virtual void handle_read(){
#ifdef DEBUG
			fprintf(stderr, "unhandled handle_read()\n");
#endif
		}
		virtual void handle_write(){
#ifdef DEBUG
			fprintf(stderr, "unhandled handle_write()\n");
#endif
		}
		virtual void handle_close(){
#ifdef DEBUG
			fprintf(stderr, "unhandled handle_close()\n");
#endif
		}
		virtual void handle_accept(){
#ifdef DEBUG
			fprintf(stderr, "unhandled handle_accept()\n");
#endif
		}

		int fileno() const {
			return this->_s->fileno();
		}

		void getsockopt(int level, int optname, void *optval, socklen_t *optlen) const {
			return this->_s->getsockopt(level, optname, optval, optlen);
		}

		void setsockopt(int level, int optname, const void *optval, socklen_t optlen){
			return this->_s->setsockopt(level, optname, optval, optlen);
		}

		void set_reuse_addr(int value){
			return this->_s->set_reuse_addr(value);
		}

		void listen(int backlog){
			return this->_s->listen(backlog);
		}

		void close(){
			return this->_s->close();
		}

		int recv(Buffer &b, int count = TCPSocket::DEFAULT_RECV_BUFSIZE){
			return this->_s->recv(b, count);
		}

		int recv(Buffer *b, int count = TCPSocket::DEFAULT_RECV_BUFSIZE){
			return this->_s->recv(b, count);
		}

		int send(const Buffer &buf, bool send_all = false) {
			return this->_s->send(buf, send_all);
		}
		int send(const Buffer &buf, int count, bool send_all = false){
			return this->_s->send(buf, count, send_all);
		}
		int send(const Buffer *buf, bool send_all = false){
			return this->_s->send(buf, send_all);
		}
		int send(const Buffer *buf, int count, bool send_all = false){
			return this->_s->send(buf, count, send_all);
		}

		void bind(unsigned short port){
			return this->_s->bind(port);
		}

		void connect(const char *name, unsigned short port){
			return this->_s->connect(name, port);
		}

		void connect(const std::string name, unsigned short port){
			return this->_s->connect(name, port);
		}

		bool is_closed(){
			return this->_s->is_closed();
		}

		TCPSocket *accept(){
			return this->_s->accept();
		}

		TCPSocket::TCPSocketState get_state() const {
			return this->_s->get_state();
		}
};
