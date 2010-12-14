#pragma once
#include <alloca.h>

#include "../AsyncDispatcher/AsyncDispatcher.h"
#include "../../libs/Buffer/VectorBuffer.h"
#include "../../libs/Mutex/Mutex.h"
#include "ClientState.h"
#include "../ClientListener.h"
#include "../Cache/ClientRetranslator.h"
class Client;

class Client : public AsyncDispatcher, public ClientListener {
public:

private:
    friend class ClientState;
    friend class ClientGetRequest;
    friend class ClientSendReply;
    void change_state(ClientState* s);

    ClientState *_state;
    ClientRetranslator *_client_retranslator;
    Buffer *_in;
    Buffer *_out;
    int _bytes_sent;
    bool _finished;

    Mutex _mutex;
public:

    Client(TCPSocket *sock);
    ~Client();
    bool readable() const;
    bool writable() const;
    void handle_read();
    void handle_write();
    void handle_close();
    
    void add_data(const Buffer *b);
    void finished();
};
