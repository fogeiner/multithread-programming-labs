#pragma once
#define DEBUG
#include <list>
#include <vector>
#include <map>
#include <exception>
#include <string>


#include <cerrno>
#include <cstring>
#include <cassert>

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "../Fd_set/Fd_set.h"
#include "../Buffer/Buffer.h"
#include "Selectable.h"

#ifdef DEBUG
#include <cstdio>
#endif

template<typename T>
inline T max(T a, T b) {
    if (a > b) return a;
    else return b;
}

class SelectException : public std::exception {
private:
    std::string _err;
    static const int ERR_MSG_MAX_LENGTH = 256;
public:

    SelectException(int err_number);
    const char *what() const throw ();

    ~SelectException() throw () {
    }
};

class TCPSocketException : public std::exception {
private:
    std::string _err;
    static const int ERR_MSG_MAX_LENGTH = 256;
public:

    TCPSocketException(const char *msg) : _err(msg) {
    }

    TCPSocketException(int err_number);
    const char *what() const throw ();

    ~TCPSocketException() throw () {
    }
};

class SocketStateException : public TCPSocketException {
public:

    SocketStateException(const char *msg) : TCPSocketException(msg) {
    }

    SocketStateException(int err_number) : TCPSocketException(err_number) {
    }
};

class SocketException : public TCPSocketException {
public:

    SocketException(const char *msg) : TCPSocketException(msg) {
    }

    SocketException(int err_number) : TCPSocketException(err_number) {
    }
};

class SockOptException : public TCPSocketException {
public:

    SockOptException(const char *msg) : TCPSocketException(msg) {
    }

    SockOptException(int err_number) : TCPSocketException(err_number) {
    }
};

class ConnectException : public TCPSocketException {
public:

    ConnectException(const char *msg) : TCPSocketException(msg) {
    }

    ConnectException(int err_number) : TCPSocketException(err_number) {
    }
};

class AcceptException : public TCPSocketException {
public:

    AcceptException(const char *msg) : TCPSocketException(msg) {
    }

    AcceptException(int err_number) : TCPSocketException(err_number) {
    }
};

class IOException : public TCPSocketException {
public:

    IOException(const char *msg) : TCPSocketException(msg) {
    }

    IOException(int err_number) : TCPSocketException(err_number) {
    }
};

class RecvException : public IOException {
public:

    RecvException(const char *msg) : IOException(msg) {
    }

    RecvException(int err_number) : IOException(err_number) {
    }
};

class SendException : public IOException {
public:

    SendException(const char *msg) : IOException(msg) {
    }

    SendException(int err_number) : IOException(err_number) {
    }
};

class BindException : public TCPSocketException {
public:

    BindException(const char *msg) : TCPSocketException(msg) {
    }

    BindException(int err_number) : TCPSocketException(err_number) {
    }
};

class ListenException : public TCPSocketException {
public:

    ListenException(const char *msg) : TCPSocketException(msg) {
    }

    ListenException(int err_number) : TCPSocketException(err_number) {
    }
};

class DNSException : public TCPSocketException {
public:

    DNSException(const char *msg) : TCPSocketException(msg) {
    }

    DNSException(int err_number) : TCPSocketException(hstrerror(err_number)) {
    }
};

class TCPSocket : public Selectable {
public:

    enum TCPSocketState {
        CREATED, CONNECTED, LISTENING, CLOSED
    };
private:

    struct Base {
        int _sock;
        int _links;
        bool _closed;

        Base(int sock, int links = 1) : _sock(sock), _links(links), _closed(true) {
        }
    };


    void increase();
    void decrease();

    TCPSocket::Base *_b;
    struct sockaddr _addr;
    TCPSocketState _state;
public:
    const static int DEFAULT_RECV_BUFSIZE = 4096;

    TCPSocket();
    TCPSocket(int sock, struct sockaddr *addr = NULL);
    TCPSocket(const TCPSocket &orig);
    ~TCPSocket();

    TCPSocket & operator=(const TCPSocket &orig);

    int fileno() const;

    void getsockopt(int level, int optname, void *optval, socklen_t *optlen) const;
    void setsockopt(int level, int optname, const void *optval, socklen_t optlen);
    void set_reuse_addr(int value);

    void listen(int backlog);
    void close();
    int recv(Buffer &b, int count = DEFAULT_RECV_BUFSIZE);
    int recv(Buffer *b, int count = DEFAULT_RECV_BUFSIZE);

    int send(const Buffer &buf, bool send_all = false);
    int send(const Buffer &buf, int count, bool send_all = false);
    int send(const Buffer *buf, bool send_all = false);
    int send(const Buffer *buf, int count, bool send_all = false);

    void bind(unsigned short port);

    void connect(const char *name, unsigned short port);
    void connect(const std::string name, unsigned short port);

    bool is_closed() const;

    TCPSocket *accept();

    TCPSocketState get_state() const;
    int peek() const;
};
