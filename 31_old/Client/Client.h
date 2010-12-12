#pragma once
#include <alloca.h>
#include "../AsyncDispatcher/AsyncDispatcher.h"
#include "ClientState.h"
class Client;
class Retranslator;
class CacheEntry;

class Client: public AsyncDispatcher {
public:

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

    CacheEntry *_ce;
    Retranslator *_r;
    int _bytes_sent;

public:


    Client(TCPSocket *sock);
    ~Client();
    void switch_to_retranslator(const Buffer *b, Retranslator *r);
    void error(std::string msg);
    bool readable() const;
    bool writable() const;
    void handle_read();
    void handle_write();
    void handle_close();
};
