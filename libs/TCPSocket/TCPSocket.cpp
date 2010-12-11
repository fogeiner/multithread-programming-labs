#include "TCPSocket.h"
//#define DEBUG

SelectException::SelectException(int err_number) {
    char buf[ERR_MSG_MAX_LENGTH];
    char *msg_ptr;
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE || defined( sun ) || defined( __sun )
    ::strerror_r(err_number, buf, sizeof (buf));
    this->_err.assign(buf);
#else
    msg_ptr = ::strerror_r(err_number, buf, sizeof (buf));
    this->_err.assign(msg_ptr);
#endif
}

EAGAINException::EAGAINException() {
    char buf[ERR_MSG_MAX_LENGTH];
    char *msg_ptr;
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE || defined( sun ) || defined( __sun )
    ::strerror_r(EAGAIN, buf, sizeof (buf));
    this->_err.assign(buf);
#else
    msg_ptr = ::strerror_r(EAGAIN, buf, sizeof (buf));
    this->_err.assign(msg_ptr);
#endif

}

const char *EAGAINException::what() const throw () {
    return _err.c_str();
}

const char *SelectException::what() const throw () {
    return this->_err.c_str();
}

void Select(std::list<Selectable*> *rlist,
        std::list<Selectable*> *wlist,
        std::list<Selectable*> *xlist, int ms_timeout) {
    std::map<int, Selectable*> fd2obj;

    Fd_set rfds, wfds, xfds;

    if (rlist != NULL)
        for (std::list<Selectable*>::iterator i = rlist->begin();
                i != rlist->end(); ++i) {
            Selectable *s = *i;
            int fd = s->fileno();
            fd2obj.insert(std::pair<int, Selectable*>(fd, s));
            rfds.set(fd);
        }

    if (wlist != NULL)
        for (std::list<Selectable*>::iterator i = wlist->begin();
                i != wlist->end(); ++i) {
            Selectable *s = *i;
            int fd = s->fileno();
            fd2obj.insert(std::pair<int, Selectable*>(fd, s));
            wfds.set(fd);
        }

    if (xlist != NULL)
        for (std::list<Selectable*>::iterator i = xlist->begin();
                i != xlist->end(); ++i) {
            Selectable *s = *i;
            int fd = s->fileno();
            fd2obj.insert(std::pair<int, Selectable*>(fd, s));
            xfds.set(fd);
        }

    int max_fd = max(max(rfds.max_fd(), wfds.max_fd()), xfds.max_fd());

    struct timeval tv;
    const int USEC_IN_SEC = 1000;
    tv.tv_sec = ms_timeout / USEC_IN_SEC;
    tv.tv_usec = ms_timeout - (ms_timeout / USEC_IN_SEC) * USEC_IN_SEC;

    struct timeval *tv_p = (ms_timeout == 0) ? NULL : &tv;

    int av_fds;
    if ((av_fds = select(max_fd + 1, &rfds.fdset(), &wfds.fdset(), &xfds.fdset(), tv_p)) == -1) {
        throw SelectException(errno);
    }
#ifdef DEBUG
    fprintf(stderr, "Select returned %d fds\n", av_fds);
#endif

    if (rlist != NULL)
        rlist->clear();
    if (wlist != NULL)
        wlist->clear();
    if (xlist != NULL)
        xlist->clear();

    for (std::map<int, Selectable*>::iterator i = fd2obj.begin();
            i != fd2obj.end(); ++i) {
        std::pair<int, Selectable*> kv = *i;
        int fd = kv.first;
        Selectable *s = kv.second;

        if (rlist != NULL)
            if (rfds.isset(fd)) {
                rlist->push_back(s);
            }

        if (wlist != NULL)
            if (wfds.isset(fd)) {
                wlist->push_back(s);
            }

        if (xlist != NULL)
            if (xfds.isset(fd)) {
                xlist->push_back(s);
            }
    }
}

TCPSocketException::TCPSocketException(int err_number) {
    char buf[ERR_MSG_MAX_LENGTH];
    char *msg_ptr;
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE || defined( sun ) || defined( __sun )
    ::strerror_r(err_number, buf, sizeof (buf));
    this->_err.assign(buf);
#else
    msg_ptr = ::strerror_r(err_number, buf, sizeof (buf));
    this->_err.assign(msg_ptr);
#endif
}

const char *TCPSocketException::what() const throw () {
    return this->_err.c_str();
}

void TCPSocket::increase() {
    if (this->_state != CLOSED) {
        assert(_b->_links != 0);
        _b->_links++;
#ifdef DEBUG
        fprintf(stderr, "socket %d increase: now %d\n", _b->_sock, _b->_links);
#endif
    }
}

void TCPSocket::decrease() {
    if (this->_state != CLOSED) {
        assert(_b->_links > 0);

        _b->_links--;

#ifdef DEBUG
        fprintf(stderr, "socket %d decrease: now %d\n", _b->_sock, _b->_links);
#endif
        if (_b->_links == 0) {
            this->close();
        }
    }
}

TCPSocket::TCPSocket() {
    int sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        throw SocketException(errno);
    }

    this->_b = new TCPSocket::Base(sock);
    this->_state = CREATED;
    this->_nonblocking = false;
#ifdef DEBUG
    fprintf(stderr, "created socket %d\n", _b->_sock);
#endif
}

TCPSocket::TCPSocket(int sock, struct sockaddr *addr) {
    this->_b = new TCPSocket::Base(sock);

    if (addr != NULL) {
        this->_addr = *addr;
    }

#ifdef DEBUG
    fprintf(stderr, "created socket %d\n", _b->_sock);
#endif
    this->_state = CREATED;
    this->_nonblocking = false;
}

TCPSocket::TCPSocket(const TCPSocket &orig) {
    this->_b = orig._b;
    this->_state = orig._state;
    this->_nonblocking = orig._nonblocking;
    this->increase();
}

TCPSocket::~TCPSocket() {
    this->decrease();
}

TCPSocket& TCPSocket::operator=(const TCPSocket &orig) {
    if (this == &orig) {
        return *this;
    }

    this->decrease();
    this->_state = orig._state;
    this->_b = orig._b;
    this->_nonblocking = orig._nonblocking;
    this->increase();
    return *this;
}

int TCPSocket::fileno() const {
    return this->_b->_sock;
}

void TCPSocket::getsockopt(int level, int optname, void *optval, socklen_t *optlen) const {
    if (::getsockopt(this->_b->_sock, level, optname, optval, optlen) == -1) {
        throw SockOptException(errno);
    }
}

void TCPSocket::setsockopt(int level, int optname, const void *optval, socklen_t optlen) {
    if (this->_state == CLOSED) {
        throw SocketStateException("Wrong state for operation");
    }

    if (::setsockopt(this->_b->_sock, level, optname, optval, optlen) == -1) {
        throw SockOptException(errno);
    }
}

void TCPSocket::set_reuse_addr(int value) {
    if (this->_state == CLOSED) {
        throw SocketStateException("Wrong state for operation");
    }
    this->setsockopt(SOL_SOCKET, SO_REUSEADDR, &value, sizeof (value));
}

void TCPSocket::listen(int backlog) {
    if (this->_state != CREATED) {
        throw SocketStateException("Wrong state for operation");
    }

    if (::listen(this->_b->_sock, backlog) == -1) {
        throw ListenException(errno);
    }
    this->_state = LISTENING;
}

void TCPSocket::close() {

    if (this->_state != CLOSED) {
#ifdef DEBUG
        fprintf(stderr, "socket %d closing\n", _b->_sock);
#endif
        ::close(_b->_sock);
        delete this->_b;
        this->_state = CLOSED;
    }
}

int TCPSocket::recv(Buffer &b, int count) {
    return this->recv(&b, count);
}

int TCPSocket::recv(Buffer *b, int count) {
    assert(count > 0);

    if ((this->_state != CONNECTED) && (this->_state != CONNECTING)) {
        throw SocketStateException("Wrong state for operation");
    }

    this->validate_connect();

    char *buf = new char[count];

    int read = ::recv(this->_b->_sock, buf, count, MSG_NOSIGNAL);
#ifdef DEBUG
    fprintf(stderr, "socket %d recv %d bytes\n", _b->_sock, read);
#endif

    if (read == -1) {
        delete[] buf;
        if (errno == EAGAIN)
            throw EAGAINException();
        else
            throw RecvException(errno);
    }

    if (read > 0) {
        b->append(buf, read);
    }

    if (read == 0) {
        this->close();
    }

    delete[] buf;
    return read;
}

int TCPSocket::send(const Buffer &buf, bool send_all) {
    return this->send(&buf, send_all);
}

int TCPSocket::send(const Buffer &buf, int count, bool send_all) {
    return this->send(&buf, count, send_all);
}

int TCPSocket::send(const Buffer *buf, bool send_all) {
    return this->send(buf, buf->size(), send_all);
}

int TCPSocket::send(const Buffer *buf, int count, bool send_all) {
    if ((this->_state != CONNECTED) && (this->_state != CONNECTING)) {
        throw SocketStateException("Wrong state for operation");
    }

    assert(buf->size() >= count);

    this->validate_connect();
    if (send_all) {
        int to_send = count;
        int sent_total = 0;
        while (to_send != sent_total) {
            int sent = ::send(this->_b->_sock, buf->buf() + sent_total,
                    to_send - sent_total, MSG_NOSIGNAL);
#ifdef DEBUG
            fprintf(stderr, "socket %d sent %d bytes\n", _b->_sock, sent);
#endif

            if (sent == -1) {
                if (errno == EAGAIN)
                    throw EAGAINException();
                else
                    throw SendException(errno);
            }

            sent_total += sent;
        }

        return sent_total;
    } else {
        int sent = ::send(this->_b->_sock, buf->buf(),
                buf->size(), MSG_NOSIGNAL);
#ifdef DEBUG
        fprintf(stderr, "socket %d sent %d bytes\n", _b->_sock, sent);
#endif

        if (sent == -1) {
            if (errno == EAGAIN)
                throw EAGAINException();
            else
                throw SendException(errno);
        }

        return sent;
    }
}

void TCPSocket::bind(unsigned short port) {
    if (this->_state != CREATED) {
        throw SocketStateException("Wrong state for operation");
    }

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof (local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(this->_b->_sock, (const sockaddr*) & local_addr, sizeof (local_addr)) == -1) {
        throw BindException(errno);
    }

#ifdef DEBUG
    fprintf(stderr, "socket %d bound to port %d\n", _b->_sock, port);
#endif
}

void TCPSocket::connect(const char *name, unsigned short port) {
    if (this->_state != CREATED) {
        throw SocketStateException("Wrong state for operation");
    }

    struct sockaddr_in remote_addr;

    const int GETHOSTBYNAME_R_BUFSIZE = 4096;
    char tmp_buf[GETHOSTBYNAME_R_BUFSIZE];

struct hostent *result;
#if defined( linux ) || defined( __linux )
    struct hostent ret;
    
    int h_errnop;

    if (gethostbyname_r(name, &ret, tmp_buf, GETHOSTBYNAME_R_BUFSIZE,
            &result, &h_errnop) == -1) {
        throw DNSException(h_errno);
    }
    
    if (result == NULL) {
        throw DNSException(h_errno);
    }
#endif

#if defined( sun ) || defined( __sun )
    struct hostent entry;
    int h_errnop;
    if ((result =  gethostbyname_r(name, &entry, tmp_buf, GETHOSTBYNAME_R_BUFSIZE,
            &h_errnop)) == NULL) {
        throw DNSException(h_errno);
    }
#endif


    memset(&remote_addr, 0, sizeof (remote_addr));
    remote_addr.sin_addr = *((in_addr *) result->h_addr_list[0]);
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(port);

    if (this->_nonblocking == false) {
        if (::connect(this->_b->_sock, (struct sockaddr*) & remote_addr, sizeof (remote_addr)) == -1) {
            if (errno == EAGAIN)
                throw EAGAINException();
            else
                throw ConnectException(errno);
        }
        this->_state = CONNECTED;
    }

    if (this->_nonblocking == true) {
        ::connect(this->_b->_sock, (struct sockaddr*) & remote_addr, sizeof (remote_addr));
        this->_state = CONNECTING;
    }

#ifdef DEBUG
    fprintf(stderr, "connected to %s port %d\n", name, port);
#endif
}

void TCPSocket::connect(const std::string name, unsigned short port) {
    return this->connect(name.c_str(), port);
}

TCPSocket *TCPSocket::accept() {
    if (this->_state != LISTENING) {
        throw SocketStateException("Wrong state for operation");
    }

    struct sockaddr addr;
    socklen_t len = sizeof (addr);
    int n_sock = ::accept(this->_b->_sock, &addr, &len);
#ifdef DEBUG
    fprintf(stderr, "socket %d accepted %d\n", this->_b->_sock, n_sock);
#endif
    if (n_sock == -1) {
        if (errno == EAGAIN)
            throw EAGAINException();
        else
            throw AcceptException(errno);
    }
    TCPSocket *s = new TCPSocket(n_sock, &addr);
    s->_state = CONNECTED;
    return s;
}

bool TCPSocket::is_closed() const {
    return this->_state == CLOSED;
}

TCPSocket::TCPSocketState TCPSocket::get_state() const {
    return this->_state;
}

/**
 * peeks for max of 1 byte
 */
int TCPSocket::peek() const {
    char b;
    int read;
    read = ::recv(this->_b->_sock, &b, 1, MSG_PEEK | MSG_NOSIGNAL);
    if (read == -1) {
        if (errno == EAGAIN)
            throw EAGAINException();
        else
            throw RecvException(errno);
    }
    return read;
}

void TCPSocket::set_nonblocking(int value) {
    if (this->_state == CLOSED) {
        throw SocketStateException("Wrong state for operation");
    }

    int retv;
    int flags;

    if (-1 == (flags = fcntl(this->_b->_sock, F_GETFL, 0))) {
        flags = 0;
    }
    retv = fcntl(this->_b->_sock, F_SETFL, value != 0 ? flags | O_NONBLOCK : flags & ~O_NONBLOCK);

    if (value != 0) {
        this->_nonblocking = true;
    } else {
        this->_nonblocking = false;
    }
    assert(retv == 0);
}

void TCPSocket::validate_connect() {
    if (this->_state == CONNECTING) {
        int error;
        socklen_t len = sizeof (error);
        this->getsockopt(SOL_SOCKET, SO_ERROR, &error, &len);
        if (error != 0) {
            // can this happen?
            if (error == EAGAIN)
                throw EAGAINException();
            else
                throw ConnectException(error);
        }
        this->_state = CONNECTED;
    }
}
