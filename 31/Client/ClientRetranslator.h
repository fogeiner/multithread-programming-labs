#pragma once
#include "ClientState.h"
class Client;

class ClientRetranslator : public ClientState {
private:

    ClientRetranslator();
public:

    static ClientState *instance();
    bool writable(const Client *c);
    bool readable(const Client *c);
    void handle_write(Client *c);

};
