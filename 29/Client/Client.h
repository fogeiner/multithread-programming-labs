#pragma once
#include <alloca.h>
#include "../../libs/AsyncDispatcher/AsyncDispatcher.h"
#include "ClientState.h"
class Client;
class Retranslator;
class CacheEntry;

class Client: public AsyncDispatcher {
public:

    enum method {
        HEAD, GET
    };

private:
    friend class ClientState;
    friend class ClientGettingRequest;
    friend class ClientError;
    friend class ClientCache;
    friend class ClientRetranslator;
    friend class Retranslator;

    void change_state(ClientState* s);

    ClientState *_state;

    Buffer *_b;

    int _bytes_sent;

    CacheEntry *_ce;
    Retranslator *_r;

    enum method _m;
public:


    Client(TCPSocket *sock);

    void retranslator(const Buffer *b, Retranslator *r);
    void error(std::string msg);
    bool readable() const;
    bool writable() const;
    void handle_read();
    void handle_write();
    void handle_close();
};
