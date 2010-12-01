#pragma once
#include <exception>
#include "ClientState.h"

class ClientBadRequestException : public std::exception {
};

class ClientGettingRequest : public ClientState {
    ClientGettingRequest();
public:

    static ClientState *instance();

    virtual bool readable(const Client *c);

    virtual bool writable(const Client *c);

    virtual void handle_read(Client *c);
    virtual void handle_close(Client *c);
};

