#pragma once

#include "../../libs/TCPSocket/TCPSocket.h"
#include "../../libs/TCPSocket/Selectable.h"
#include "Async.h"
#include <list>
#include <cassert>

#ifdef DEBUG
#include <cstdio>
#endif

class AsyncDispatcher : public Selectable, public Async {
private:
    friend class SelectTask;
    static std::list<AsyncDispatcher*> _sockets;
    TCPSocket *_s;
public:
    AsyncDispatcher();
    AsyncDispatcher(int sock);
    AsyncDispatcher(TCPSocket *socket);
    AsyncDispatcher(const AsyncDispatcher &orig);
    AsyncDispatcher & operator=(const AsyncDispatcher &orig);
    ~AsyncDispatcher();

    static void loop(int timeout_ms = 0);

    bool readable() const;
    bool writable() const;

    void handle_read();
    void handle_write();
    void handle_close();
    void handle_accept();
    void handle_connect();

    int fileno() const;

    void getsockopt(int level, int optname, void *optval, socklen_t *optlen) const;
    void setsockopt(int level, int optname, const void *optval, socklen_t optlen);

    void set_reuse_addr(int value);

    void listen(int backlog);

    void close();
    int recv(Buffer &b, int count = TCPSocket::DEFAULT_RECV_BUFSIZE);
    int recv(Buffer *b, int count = TCPSocket::DEFAULT_RECV_BUFSIZE);

    int send(const Buffer &buf, bool send_all = false);
    int send(const Buffer &buf, int count, bool send_all = false);
    int send(const Buffer *buf, bool send_all = false);
    int send(const Buffer *buf, int count, bool send_all = false);

    void bind(unsigned short port);

    void connect(const char *name, unsigned short port);
    void connect(const std::string name, unsigned short port);
    void validate_connect();
    bool is_closed();
    TCPSocket *accept();
    TCPSocket::TCPSocketState get_state() const;
};
