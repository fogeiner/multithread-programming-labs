#pragma once
#include <alloca.h>

#include "../AsyncDispatcher/AsyncDispatcher.h"
#include "../../libs/Buffer/VectorBuffer.h"

#include "ClientState.h"
#include "../DownloadListener.h"
class Client;

class Client : public AsyncDispatcher, public DownloadListener {
public:

private:
    friend class ClientState;
    friend class ClientGetRequest;
    friend class ClientSendReply;
    void change_state(ClientState* s);
    ClientState *_state;
    Buffer *_in;
    Buffer *_out;
    int _bytes_sent;
    bool _finished;
    bool _cancelled;
public:

    Client(TCPSocket *sock);
    ~Client();
    bool readable() const;
    bool writable() const;
    void handle_read();
    void handle_write();
    void handle_close();
    
    void add_data(const Buffer *b, bool absolute = false);
    void finished();
    void cancelled();
    bool is_finished() const;
    bool is_cancelled() const;
};
