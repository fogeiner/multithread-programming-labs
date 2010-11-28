#include "AsyncDispatcher.h"

std::list<AsyncDispatcher*> AsyncDispatcher::_sockets;

AsyncDispatcher::AsyncDispatcher(){
#ifdef DEBUG
	fprintf(stderr, "AsyncDispatcher(); adding to lst\n");
#endif
	this->_s = new TCPSocket();
	this->_sockets.push_back(this);
}

AsyncDispatcher::AsyncDispatcher(TCPSocket *socket){
#ifdef DEBUG
	fprintf(stderr, "AsyncDispatcher(TCPSocket *socket); adding to lst\n");
#endif
	this->_s = socket;
	this->_sockets.push_back(this);
}

AsyncDispatcher::AsyncDispatcher(int sock){
#ifdef DEBUG
	fprintf(stderr, "AsyncDispatcher(int sock); adding to lst\n");
#endif
	this->_s = new TCPSocket(sock);
	this->_sockets.push_back(this);
}

AsyncDispatcher::AsyncDispatcher(const AsyncDispatcher &orig){
	assert(false);
}

AsyncDispatcher& AsyncDispatcher::operator=(const AsyncDispatcher &orig){
	assert(false);
}

AsyncDispatcher::~AsyncDispatcher(){
#ifdef DEBUG
	fprintf(stderr, "~AsyncDispatcher(); removing from lst\n");
#endif
	this->_sockets.remove(this);
	delete this->_s;
}

void AsyncDispatcher::loop(int timeout_ms){
	while(1) {
		std::list<Selectable*> rlist, wlist, xlist;
		for(std::list<AsyncDispatcher*>::iterator i = _sockets.begin();
				i != _sockets.end(); ++i){
			AsyncDispatcher *s = *i;
			if (s->readable()){
				rlist.push_back(s->_s);
			}
			if (s->writable()){
				wlist.push_back(s->_s);
			}
			if (s->readable() || s->writable()){
				xlist.push_back(s->_s);
			}
		}

		Select(&rlist, &wlist, &xlist, timeout_ms);

		for(std::list<Selectable*>::iterator i = rlist.begin();
				i != rlist.end(); ++i){
			AsyncDispatcher *ad = NULL;
			for(std::list<AsyncDispatcher*>::iterator d = _sockets.begin();
					d != _sockets.end(); ++d){
				if ( *i == (*d)->_s ){
					ad = *d;
					break;
				}
			}
			
			assert(ad != NULL);
			if (ad->_s->get_state() == TCPSocket::CONNECTED){
				if (ad->_s->peek() == 0){
					ad->handle_close();
				} else {
					ad -> handle_read();
				}
			}

			if (ad->_s->get_state() == TCPSocket::LISTENING){
				ad -> handle_accept();
			}
		}

		for(std::list<Selectable*>::iterator i = wlist.begin();
				i != wlist.end(); ++i){
			AsyncDispatcher *ad = NULL;
			for(std::list<AsyncDispatcher*>::iterator d = _sockets.begin();
					d != _sockets.end(); ++d){
				if ( *i == (*d)->_s ){
					ad = *d;
					break;
				}
			}
			assert(ad != NULL);
			ad->handle_write();
		}
	}
}

bool AsyncDispatcher::readable() const {
#ifdef DEBUG
	fprintf(stderr, "default readable()\n");
#endif
	return false;
}
bool AsyncDispatcher::writable() const{
#ifdef DEBUG
	fprintf(stderr, "default writable()\n");
#endif
	return false;
}

void AsyncDispatcher::handle_read(){
#ifdef DEBUG
	fprintf(stderr, "unhandled handle_read()\n");
#endif
}
void AsyncDispatcher::handle_write(){
#ifdef DEBUG
	fprintf(stderr, "unhandled handle_write()\n");
#endif
}
void AsyncDispatcher::handle_close(){
#ifdef DEBUG
	fprintf(stderr, "unhandled handle_close()\n");
#endif
}
void AsyncDispatcher::handle_accept(){
#ifdef DEBUG
	fprintf(stderr, "unhandled handle_accept()\n");
#endif
}

int AsyncDispatcher::fileno() const {
	return this->_s->fileno();
}

void AsyncDispatcher::getsockopt(int level, int optname, void *optval, socklen_t *optlen) const {
	this->_s->getsockopt(level, optname, optval, optlen);
}

void AsyncDispatcher::setsockopt(int level, int optname, const void *optval, socklen_t optlen){
	this->_s->setsockopt(level, optname, optval, optlen);
}

void AsyncDispatcher::set_reuse_addr(int value){
	this->_s->set_reuse_addr(value);
}

void AsyncDispatcher::listen(int backlog){
	this->_s->listen(backlog);
}

void AsyncDispatcher::close(){
	this->_s->close();
	delete this;
}

int AsyncDispatcher::recv(Buffer &b, int count){
	return this->_s->recv(b, count);
}

int AsyncDispatcher::recv(Buffer *b, int count){
	return this->_s->recv(b, count);
}

int AsyncDispatcher::send(const Buffer &buf, bool send_all) {
	return this->_s->send(buf, send_all);
}
int AsyncDispatcher::send(const Buffer &buf, int count, bool send_all){
	return this->_s->send(buf, count, send_all);
}
int AsyncDispatcher::send(const Buffer *buf, bool send_all){
	return this->_s->send(buf, send_all);
}
int AsyncDispatcher::send(const Buffer *buf, int count, bool send_all){
	return this->_s->send(buf, count, send_all);
}

void AsyncDispatcher::bind(unsigned short port){
	return this->_s->bind(port);
}

void AsyncDispatcher::connect(const char *name, unsigned short port){
	this->_s->connect(name, port);
}

void AsyncDispatcher::connect(const std::string name, unsigned short port){
	this->_s->connect(name, port);
}

bool AsyncDispatcher::is_closed(){
	return this->_s->is_closed();
}

TCPSocket *AsyncDispatcher::accept(){
	return this->_s->accept();
}

TCPSocket::TCPSocketState AsyncDispatcher::get_state() const {
	return this->_s->get_state();
}
