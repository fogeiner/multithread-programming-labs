#pragma once
#include "ClientState.h"

class Client;

class ClientSendReply : public ClientState {
public:
    virtual bool readable(const Client *c);
    virtual bool writable(const Client *c);
    virtual void handle_read(Client *c);
    virtual void handle_write(Client *c);
    virtual void handle_close(Client *c);
    virtual void handle_connect(Client *c);
};