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
}

void DirectRetranslator::download_add_data(Retranslator *r, const Buffer *b) {
    Logger::debug("DirectRetranslator::download_add_data()");
    r->_mutex.lock();
    if (r->clients_count() == 0) {
        Logger::debug("DirectRetranslator::download_add_data() no clients; cancelling download");

        r->_download_listener->cancel();

        r->_mutex.unlock();
        delete r;
        return;
    }

    // add to clients
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->add_data(b);
    }
    r->_mutex.unlock();
}

void DirectRetranslator::download_finished(Retranslator *r) {
    Logger::debug("DirectRetranslator::download_finished()");
    r->_mutex.lock();
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->finished();
    }
    r->_mutex.unlock();
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
    r->_mutex.lock();
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->finished();
    }
    r->_mutex.unlock();
    delete r;
}
