#include "AsyncDispatcher.h"
#include "../Logger/Logger.h"
#define DEBUG
std::list<AsyncDispatcher*> AsyncDispatcher::_sockets;

AsyncDispatcher::AsyncDispatcher() {
Logger::debug("AsyncDispatcher(); adding to lst");

    this->_s = new TCPSocket();
    this->_s->set_nonblocking(1);
    this->_sockets.push_back(this);
}

AsyncDispatcher::AsyncDispatcher(TCPSocket *socket) {
Logger::debug("AsyncDispatcher(TCPSocket *socket); adding to lst");

    this->_s = socket;
    this->_s->set_nonblocking(1);
    this->_sockets.push_back(this);
}

AsyncDispatcher::AsyncDispatcher(int sock) {
    Logger::debug("AsyncDispatcher(int sock); adding to lst");

    this->_s = new TCPSocket(sock);
    this->_s->set_nonblocking(1);
    this->_sockets.push_back(this);
}

AsyncDispatcher::AsyncDispatcher(const AsyncDispatcher &orig) {
    assert(false);
}

AsyncDispatcher& AsyncDispatcher::operator=(const AsyncDispatcher &orig) {
    assert(false);
}

AsyncDispatcher::~AsyncDispatcher() {
    Logger::debug("~AsyncDispatcher(); removing from lst\n");

    this->_sockets.remove(this);
    delete this->_s;
}

void AsyncDispatcher::loop(int timeout_ms) {
    while (_sockets.size() != 0) {
        Logger::debug("loop next");
        std::list<Selectable*> rlist, wlist; //, xlist;

        for (std::list<AsyncDispatcher*>::iterator i = _sockets.begin();
                i != _sockets.end(); ++i) {
            AsyncDispatcher *s = *i;
            if (s->readable()) {
                rlist.push_back(s->_s);

            }
            if (s->writable()) {
                wlist.push_back(s->_s);
            }
            //    if (s->readable() || s->writable()) {
            //        xlist.push_back(s->_s);
            //    }
        }

        Select(&rlist, &wlist, NULL/*&xlist*/, timeout_ms);

        for (std::list<Selectable*>::iterator i = rlist.begin();
                i != rlist.end(); ++i) {
            AsyncDispatcher *ad = NULL;
            for (std::list<AsyncDispatcher*>::iterator d = _sockets.begin();
                    d != _sockets.end(); ++d) {
                if (*i == (*d)->_s) {
                    ad = *d;
                    break;
                }
            }

            if (ad->_s->get_state() == TCPSocket::LISTENING) {
                ad -> handle_accept();
            }
            assert(ad != NULL);
            if (ad->_s->get_state() == TCPSocket::CONNECTED) {
                if (ad->_s->peek() == 0) {
                    ad->handle_close();
                } else {
                    ad -> handle_read();
                }
            }
        }

        for (std::list<Selectable*>::iterator i = wlist.begin();
                i != wlist.end(); ++i) {
            AsyncDispatcher *ad = NULL;
            for (std::list<AsyncDispatcher*>::iterator d = _sockets.begin();
                    d != _sockets.end(); ++d) {
                if (*i == (*d)->_s) {
                    ad = *d;
                    break;
                }
            }
            assert(ad != NULL);
            if (ad->_s->get_state() == TCPSocket::CONNECTED) {
                ad->handle_write();
            }

            if (ad->_s->get_state() == TCPSocket::CONNECTING) {
                // in case connect failed it wont'work
                //                ad->_s->validate_connect();
                ad->handle_connect();
            }
        }
    }
}

bool AsyncDispatcher::readable() const {
    Logger::debug("default readable()\n");

    return false;
}

bool AsyncDispatcher::writable() const {
    Logger::debug("default writable()\n");
    return false;
}

void AsyncDispatcher::handle_read() {
    Logger::debug("unhandled handle_read()\n");

}

void AsyncDispatcher::handle_write() {
    Logger::debug("unhandled handle_write()\n");
}

void AsyncDispatcher::handle_close() {
    Logger::debug("unhandled handle_close()\n");
}

void AsyncDispatcher::handle_accept() {
    Logger::debug("unhandled handle_accept()\n");
}

void AsyncDispatcher::handle_connect() {
    Logger::debug("unhandled handle_connect()\n");

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
    this->_s->close();
    delete this;
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

