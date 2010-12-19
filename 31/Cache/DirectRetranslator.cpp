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
    r->_finished_clients_mutex.lock();
    r->_finished_clients.push_back(client_listener);
    r->_finished_clients_mutex.unlock();
}

void DirectRetranslator::download_add_data(Retranslator *r, const Buffer *b) {
    Logger::debug("DirectRetranslator::download_add_data()");
    r->_clients_mutex.lock();

    r->_finished_clients_mutex.lock();
    for (std::list<ClientListener*>::iterator i = r->_finished_clients.begin();
            i != r->_finished_clients.end(); ++i) {
        r->_clients.remove(*i);
    }
    r->_finished_clients.clear();
    r->_finished_clients_mutex.unlock();

    if (r->clients_count() == 0) {
        Logger::debug("DirectRetranslator::download_add_data() no clients; cancelling download");

        r->_download_listener->cancel();
        r->_download_listener = DummyDownloadListener::instance();

        r->_clients_mutex.unlock();
        delete r;
        return;
    }

    // add to clients
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->add_data(b);
    }
    r->_clients_mutex.unlock();
}

void DirectRetranslator::download_finished(Retranslator *r) {
    Logger::debug("DirectRetranslator::download_finished()");
    r->_clients_mutex.lock();

    r->_finished_clients_mutex.lock();
    for (std::list<ClientListener*>::iterator i = r->_finished_clients.begin();
            i != r->_finished_clients.end(); ++i) {
        r->_clients.remove(*i);
    }
    r->_finished_clients.clear();
    r->_finished_clients_mutex.unlock();


    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->finished();
    }
    r->_clients_mutex.unlock();
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
    r->_clients_mutex.lock();

    r->_finished_clients_mutex.lock();
    for (std::list<ClientListener*>::iterator i = r->_finished_clients.begin();
            i != r->_finished_clients.end(); ++i) {
        r->_clients.remove(*i);
    }
    r->_finished_clients.clear();
    r->_finished_clients_mutex.unlock();


    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->finished();
    }
    r->_clients_mutex.unlock();
    delete r;
}
