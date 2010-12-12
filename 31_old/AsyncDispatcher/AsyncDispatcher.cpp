#include "AsyncDispatcher.h"
#include "../../libs/Logger/Logger.h"
//#define DEBUG
std::list<AsyncDispatcher*> AsyncDispatcher::_sockets;
SignalPipe AsyncDispatcher::_signal_pipe;
Mutex AsyncDispatcher::_sockets_mutex(Mutex::ERRORCHECK_MUTEX);

AsyncDispatcher::AsyncDispatcher() {
#ifdef DEBUG
    fprintf(stderr, "AsyncDispatcher(); adding to lst\n");
#endif
    this->_s = new TCPSocket();
    this->_s->set_nonblocking(1);
    _sockets_mutex.lock();
    this->_sockets.push_back(this);
    _sockets_mutex.unlock();
    this->activate();
}

AsyncDispatcher::AsyncDispatcher(TCPSocket *socket) {
#ifdef DEBUG
    fprintf(stderr, "AsyncDispatcher(TCPSocket *socket); adding to lst\n");
#endif
    this->_s = socket;
    this->_s->set_nonblocking(1);
    _sockets_mutex.lock();
    this->_sockets.push_back(this);
    _sockets_mutex.unlock();
    this->activate();
}

AsyncDispatcher::AsyncDispatcher(int sock) {
#ifdef DEBUG
    fprintf(stderr, "AsyncDispatcher(int sock); adding to lst\n");
#endif
    this->_s = new TCPSocket(sock);
    this->_s->set_nonblocking(1);
    _sockets_mutex.lock();
    this->_sockets.push_back(this);
    _sockets_mutex.unlock();
    this->activate();
}

AsyncDispatcher::AsyncDispatcher(const AsyncDispatcher &orig) {
    assert(false);
}

AsyncDispatcher& AsyncDispatcher::operator=(const AsyncDispatcher &orig) {
    assert(false);
}

AsyncDispatcher::~AsyncDispatcher() {
#ifdef DEBUG
    fprintf(stderr, "~AsyncDispatcher(); removing from lst\n");
#endif

    delete this->_s;
}

bool AsyncDispatcher::readable() const {

    Logger::debug("default readable()");
    return false;
}

bool AsyncDispatcher::writable() const {

    Logger::debug("default writable()");
    return false;
}

void AsyncDispatcher::handle_read() {

    Logger::debug("unhandled handle_read()");
    this->activate();
}

void AsyncDispatcher::handle_write() {

    Logger::debug("unhandled handle_write()");
    this->activate();
}

void AsyncDispatcher::handle_close() {

    Logger::debug("unhandled handle_close()");
    this->activate();
}

void AsyncDispatcher::handle_accept() {

    Logger::debug("unhandled handle_accept()\n");
    this->activate();
}

void AsyncDispatcher::handle_connect() {

    Logger::debug("unhandled handle_connect()\n");
    this->activate();
}

int AsyncDispatcher::fileno() const {
    return this->_s->fileno();
}

void AsyncDispatcher::getsockopt(int level, int optname, void *optval, socklen_t *optlen) const {
    this->_s->getsockopt(level, optname, optval, optlen);
}

void AsyncDispatcher::setsockopt(int level, int optname, const void *optval, socklen_t optlen) {
    this->_s->setsockopt(level, optname, optval, optlen);
}

void AsyncDispatcher::set_reuse_addr(int value) {
    this->_s->set_reuse_addr(value);
}

void AsyncDispatcher::listen(int backlog) {
    this->_s->listen(backlog);
}

void AsyncDispatcher::close() {
    _sockets_mutex.lock();
    this->_s->close();
    _sockets_mutex.unlock();
}

int AsyncDispatcher::recv(Buffer &b, int count) {
    return this->_s->recv(b, count);
}

int AsyncDispatcher::recv(Buffer *b, int count) {
    return this->_s->recv(b, count);
}

int AsyncDispatcher::send(const Buffer &buf, bool send_all) {
    return this->_s->send(buf, send_all);
}

int AsyncDispatcher::send(const Buffer &buf, int count, bool send_all) {
    return this->_s->send(buf, count, send_all);
}

int AsyncDispatcher::send(const Buffer *buf, bool send_all) {
    return this->_s->send(buf, send_all);
}

int AsyncDispatcher::send(const Buffer *buf, int count, bool send_all) {
    return this->_s->send(buf, count, send_all);
}

void AsyncDispatcher::bind(unsigned short port) {
    return this->_s->bind(port);
}

void AsyncDispatcher::connect(const char *name, unsigned short port) {
    this->_s->connect(name, port);
}

void AsyncDispatcher::connect(const std::string name, unsigned short port) {
    this->_s->connect(name, port);
}

bool AsyncDispatcher::is_closed() {
    return this->_s->is_closed();
}

TCPSocket *AsyncDispatcher::accept() {
    return this->_s->accept();
}

TCPSocket::TCPSocketState AsyncDispatcher::get_state() const {
    return this->_s->get_state();
}

void AsyncDispatcher::validate_connect() {
    this->_s->validate_connect();
}

void AsyncDispatcher::activate() {
    this->_on = true;
    this->_signal_pipe.signal();
}

void AsyncDispatcher::deactivate() {
    this->_on = false;
}

bool AsyncDispatcher::is_active() const {
    return this->_on;
}