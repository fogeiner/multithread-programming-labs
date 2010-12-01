#pragma once

#include "ClientState.h"

class ClientError : public ClientState {
    ClientError();
public:

    static ClientState *instance();

    virtual bool readable(const Client *c);

    virtual bool writable(const Client *c);
    virtual void handle_write(Client *c);
};