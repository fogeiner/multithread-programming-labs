#pragma once
#include <alloca.h>
#include "../../libs/AsyncDispatcher/AsyncDispatcher.h"

class Client;
class Retranslator;
class CacheEntry;

class ClientState {
public:

    virtual bool readable(const Client *c);


    virtual bool writable(const Client *c);

    virtual void handle_read(Client *c);

    virtual void handle_write(Client *c);

    virtual void handle_close(Client *c);

    virtual void handle_connect(Client *c);

protected:
    void change_state(Client *c, ClientState *s);
};

class ClientError : public ClientState {
    ClientError();
public:

    static ClientState *instance();

    virtual bool readable(const Client *c);

    virtual bool writable(const Client *c);
    virtual void handle_write(Client *c);
};

class Client: public AsyncDispatcher {
public:

    enum method {
        HEAD, GET
    };

private:
    friend class ClientState;
    friend class ClientGettingRequest;
    friend class ClientError;
    friend class ClientCache;
    friend class ClientRetranslator;
    friend class Retranslator;
    void change_state(ClientState* s);
    ClientState *_state;
    int _bytes_sent;
    Buffer *_b;
    Retranslator *_r;
    CacheEntry *_ce;
    enum method _m;
public:


    Client(TCPSocket *sock);

    void error(const char *msg);
    void error(std::string msg);
    bool readable() const;
    bool writable() const;
    void handle_read();
    void handle_write();
    void handle_close();
};






// ------------------ClientGettingRequest---------------------------

class ClientGettingRequest : public ClientState {
    ClientGettingRequest();
public:

    static ClientState *instance();

    virtual bool readable(const Client *c);

    virtual bool writable(const Client *c);

    virtual void handle_read(Client *c);
    virtual void handle_close(Client *c);
};


// ------------------ClientRetranslator--------------

class ClientRetranslator : public ClientState {
private:

    ClientRetranslator();
public:

    static ClientState *instance();

    bool writable(const Client *c);

    void handle_write(Client *c);

};
// ------------------ClientCache---------------------

class ClientCache : public ClientState {
private:

    ClientCache();
public:

    static ClientState *instance();

    bool readable(const Client *c);

    bool writable(const Client *c);

    void handle_write(Client *c);

};
