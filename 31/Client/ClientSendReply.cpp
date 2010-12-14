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
    return c->_out->size() > 0 || c->is_finished();
}

void ClientSendReply::handle_write(Client *c) {
    Logger::debug("ClientSendReply::handle_write");
    try {
        int sent = c->send(c->_out);
        c->_bytes_sent += sent;

        c->_mutex.lock();
        c->_out->drop_first(sent);
        c->_mutex.unlock();

        if(c->is_finished() && c->_out->size() == 0){
            c->_client_retranslator->client_finished();
            c->close();
        }
        
    } catch (SendException &ex) {
        Logger::debug("ClientSendReply::handle_write() SendException");
    }
}
