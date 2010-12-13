#include "ClientSendReply.h"

#include "../../libs/Logger/Logger.h"
#include "Client.h"

ClientState *ClientSendReply::instance() {
    static ClientSendReply state;
    return &state;
}

bool ClientSendReply::readable(const Client *c) {
    return false;
}

bool ClientSendReply::writable(const Client *c) {
    // depends on _out Buffer
    return c->_out > 0 || c->is_finished();
}

void ClientSendReply::handle_write(Client *c) {
    Logger::debug("ClientSendReply::handle_write");
    try {
        c->_bytes_sent += c->send(c->_out);
        c->_out->drop_first(c->_bytes_sent);

        if(c->_out->size() == 0 && c->is_finished()){
            Cache::client_finished(c->_key, c);
        }
    } catch (SendException &ex) {
        Logger::debug("ClientSendReply::handle_write() SendException");
    }
}
