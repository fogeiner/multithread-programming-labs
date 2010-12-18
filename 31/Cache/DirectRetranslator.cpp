#include "DirectRetranslator.h"
#include "../../libs/Logger/Logger.h"
#include <cassert>

RetranslatorState *DirectRetranslator::instance() {
    static DirectRetranslator state;
    return &state;
}

void DirectRetranslator::add_client(Retranslator *r, ClientListener *client_listener) {
    Logger::debug("DirectRetranslator::add_client()");
    assert(false);
}

void DirectRetranslator::client_finished(Retranslator *r, ClientListener *client_listener) {
    Logger::debug("DirectRetranslator::client_finished()");

    r->delete_client(client_listener);
    if (r->clients_count() == 0) {
        r->_download_listener->cancel();
        delete r;
    }
}

void DirectRetranslator::download_add_data(Retranslator *r, const Buffer *b) {
    Logger::debug("DirectRetranslator::download_add_data()");
    // add to clients
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->add_data(b);
    }
}

void DirectRetranslator::download_finished(Retranslator *r) {
    Logger::debug("DirectRetranslator::download_finished()");
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->finished();
    }
    delete r;
}

void DirectRetranslator::download_connect_failed(Retranslator *r) {
    Logger::debug("DirectRetranslator::download_connect_failed()");
    assert(false);
}

void DirectRetranslator::download_send_failed(Retranslator *r) {
    Logger::debug("DirectRetranslator::download_send_failed()");
    assert(false);
}

void DirectRetranslator::download_recv_failed(Retranslator *r) {
    Logger::debug("DirectRetranslator::download_recv_failed()");
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->finished();
    }
    delete r;
}
