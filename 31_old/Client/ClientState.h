#pragma once

class Client;
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
