#pragma once
#include <alloca.h>

#include "../AsyncDispatcher/AsyncDispatcher.h"
#include "ClientState.h"
class Client;

class Client : public AsyncDispatcher {
public:

private:
    friend class ClientState;
    void change_state(ClientState* s);
    ClientState *_state;
public:

    Client(TCPSocket *sock);
    ~Client();
    bool readable() const;
    bool writable() const;
    void handle_read();
    void handle_write();
    void handle_close();
};
