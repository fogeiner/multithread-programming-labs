#pragma once
#include "ClientState.h"

class Client;
class ClientGetRequest : public ClientState {
public:
    static ClientState *instance();
    virtual bool readable(const Client *c);
    virtual bool writable(const Client *c);
    virtual void handle_read(Client *c);
    virtual void handle_close(Client *c);
};