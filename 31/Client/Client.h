#pragma once
#include <alloca.h>

#include "../AsyncDispatcher/AsyncDispatcher.h"
#include "../../libs/Buffer/VectorBuffer.h"
#include "../../libs/Mutex/Mutex.h"

#include "../ClientListener.h"
#include "../Cache/ClientRetranslator.h"
class Client;

class Client : public AsyncDispatcher, public ClientListener {
public:

private:

    ClientRetranslator *_client_retranslator;
    bool _got_request;
    bool _no_reply;
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
    void finished(bool no_reply = false);
};
