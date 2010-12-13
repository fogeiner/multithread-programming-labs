#pragma once
#include "ClientState.h"
class Client;

class ClientCache : public ClientState {
private:

    ClientCache();
public:

    static ClientState *instance();

    bool readable(const Client *c);

    bool writable(const Client *c);

    void handle_write(Client *c);

};
