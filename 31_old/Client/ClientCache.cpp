#include "ClientCache.h"
#include "Client.h"
#include "../../libs/Logger/Logger.h"
#include "../Cache/Cache.h"
ClientCache::ClientCache() {
}

ClientState *ClientCache::instance() {
    static ClientCache cc;
    return &cc;
}

bool ClientCache::readable(const Client *c) {
    return false;
}

bool ClientCache::writable(const Client *c) {
    return c->is_active() && (c->_ce->is_download_finished() || c->_bytes_sent < c->_ce->data_size());
}

void ClientCache::handle_write(Client *c) {
    Buffer *aux_b = NULL;
    try {
        const int K16 = 16 * 1024;
        Buffer *b = c->_ce->get_buffer();
        int buf_size = b->size();
        aux_b = b->subbuf(c->_bytes_sent, (c->_bytes_sent + K16 < buf_size ? c->_bytes_sent + K16 : buf_size));
        c->_bytes_sent += c->send(aux_b);
        delete aux_b;
        Logger::debug("ClientCache; sent %d bytes, remains %d", c->_bytes_sent, buf_size - c->_bytes_sent);
        if (c->_ce->is_download_finished() && c->_bytes_sent == c->_ce->data_size()) {
            c->_ce->remove_client(c);
            c->close();
        }
    } catch (SendException &ex) {
        Logger::error("SendException caught");
        Logger::error(ex.what());
        if(aux_b != NULL){
            delete aux_b;
        }
        c->_ce->remove_client(c);
        c->close();
    }
}
