#include <list>

#include "CacheRetranslator.h"
#include "../../libs/Logger/Logger.h"
#include "Cache.h"

RetranslatorState *CacheRetranslator::instance() {
    static CacheRetranslator state;
    return &state;
}

void CacheRetranslator::add_client(Retranslator *r, ClientListener *client_listener) {
    Logger::debug("CacheRetranslator::add_client()");
    client_listener->add_data(r->_ce.data());
    r->add_client(client_listener);

}

void CacheRetranslator::client_finished(Retranslator *r, ClientListener *client_listener) {
    Logger::debug("CacheRetranslator::client_finished()");
    r->delete_client(client_listener);
    if (r->clients_count() == 0) {
        r->_download_listener->cancel();
        Cache::drop(r->_request.url);
        delete this;
    }
}

void CacheRetranslator::download_add_data(Retranslator *r, const Buffer *b) {
    Logger::debug("CacheRetranslator::download_add_data()");
}

void CacheRetranslator::download_finished(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_finished()");
    r->_ce.cached();
}

void CacheRetranslator::download_connect_failed(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_connect_failed()");
    Cache::drop(r->_request.url);
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        Cache::request(Cache::HTTP_SERVICE_UNAVAILABLE, *i);
    }
    delete this;
}

void CacheRetranslator::download_send_failed(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_send_failed()");
    Cache::drop(r->_request.url);
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        Cache::request(Cache::HTTP_INTERNAL_ERROR, *i);
    }
    delete this;
}

void CacheRetranslator::download_recv_failed(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_recv_failed()");
    Cache::drop(r->_request.url);
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->finished(true);
    }
    delete this;
}