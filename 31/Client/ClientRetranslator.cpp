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
    return c->_b->size() >= 0;
}

bool ClientRetranslator::readable(const Client* c) {
    return false;
}
#include <cstdio>
void ClientRetranslator::handle_write(Client *c) {
    Logger::debug("Retranslating to client");


    try {
        if (c->_b->size() > 0) {
            int sent;
            sent = c->send(c->_b);
            c->_b->drop_first(sent);
        }
        
        if (c->_r->is_download_finished() && c->_b->size() == 0) {
            Logger::debug("Retranslating to client finished");
            c->_r->remove_client(c);
            c->close();
        }
    } catch (SendException &ex) {
        fprintf(stderr, "SendException");
        Logger::debug("SendException caught");
        Logger::debug(ex.what());
        c->_r->remove_client(c);
        c->close();
    } catch(EAGAINException &ex){

        fprintf(stderr, "EAGAINException");
    }
}

