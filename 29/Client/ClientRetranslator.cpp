#include "ClientRetranslator.h"
#include "Client.h"
#include "../../libs/Logger/Logger.h"
#include "../Retranslator/Retranslator.h"

ClientRetranslator::ClientRetranslator() {
}

ClientState *ClientRetranslator::instance() {
    static ClientRetranslator cr;
    return &cr;
}

bool ClientRetranslator::writable(const Client *c) {
    return c->_b->size() > 0;
}

void ClientRetranslator::handle_write(Client *c) {
    Logger::debug("Retranslating to client");
    int sent;
    sent = c->send(c->_b);
    c->_b->drop_first(sent);

    if (c->_r->is_download_finished() && c->_b->size() == 0) {
        Logger::debug("Retranslating to client finished");
        c->close();
    }
}

