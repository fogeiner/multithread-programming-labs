#include "ClientError.h"
#include "Client.h"
ClientError::ClientError() {
}

ClientState *ClientError::instance() {
    static ClientError cbr;
    return &cbr;
}

bool ClientError::readable(const Client *c) {
    return false;
}

bool ClientError::writable(const Client *c) {
    return c->_b->size() >= 0;
}

void ClientError::handle_write(Client *c) {
    try {
        int sent = c->send(c->_b);
        c->_b->drop_first(sent);
        if (c->_b->size() == 0) {
            c->close();
        }
    } catch (SendException &ex) {
        c->close();
    }
}
