#pragma once
#include "ClientState.h"
#include "../Cache/Cache.h"

class Client;

class ClientSendReply : public ClientState {
public:
    static ClientState *instance();
    virtual bool readable(const Client *c);
    virtual bool writable(const Client *c);
    virtual void handle_write(Client *c);
};