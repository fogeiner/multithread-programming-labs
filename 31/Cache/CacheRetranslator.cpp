#include <list>
#include <string>
#include <sstream>
#include "CacheRetranslator.h"
#include "DirectRetranslator.h"
#include "../../libs/Logger/Logger.h"
#include "Cache.h"

RetranslatorState *CacheRetranslator::instance() {
    static CacheRetranslator state;
    return &state;
}

void CacheRetranslator::add_client(Retranslator *r, ClientListener *client_listener) {
    Logger::debug("CacheRetranslator::add_client()");

    client_listener->add_data(r->_ce.data());
    r->_clients.push_back(client_listener);
}

void CacheRetranslator::client_finished(Retranslator *r, ClientListener *client_listener) {
    Logger::debug("CacheRetranslator::client_finished()");

    r->delete_client(client_listener);
    if (r->clients_count() == 0) {
        r->_download_listener->cancel();

        if (r->_ce.get_state() != CacheEntry::CACHED)
            Cache::drop(r->_request.url);

        delete r;
    }
}

void CacheRetranslator::download_add_data(Retranslator *r, const Buffer *b) {
    Logger::debug("CacheRetranslator::download_add_data()");

    // add to CacheEntry
    r->_ce.add_data(b);
    // add to clients
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->add_data(b);
    }
    // strlen("HTTP/1.x 200") == 12
    if (r->_response_code_received == false && b->size() >= 12) {
        r->_response_code_received = true;

        std::string response = *b;
        std::string protocol;
        std::string reply_code;
        std::istringstream iss(response, std::istringstream::in);
        iss >> protocol;
        iss >> reply_code;

        if ((protocol != "HTTP/1.0" && protocol != "HTTP/1.1") && reply_code != "200") {
            Logger::debug("CacheRetranslator::download_add_data() Switching to DirectRetranslator");
            r->change_state(DirectRetranslator::instance());
        }
    }
}

void CacheRetranslator::download_finished(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_finished()");

    r->_ce.cached();
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->finished(true);
    }
    delete r;
}

void CacheRetranslator::download_connect_failed(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_connect_failed()");

    Cache::drop(r->_request.url);
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        Cache::request(Cache::HTTP_SERVICE_UNAVAILABLE, *i);
    }
    delete r;
}

void CacheRetranslator::download_send_failed(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_send_failed()");

    Cache::drop(r->_request.url);
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        Cache::request(Cache::HTTP_INTERNAL_ERROR, *i);
    }
    delete r;
}

void CacheRetranslator::download_recv_failed(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_recv_failed()");
    
    Cache::drop(r->_request.url);
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->finished(true);
    }
    delete r;
}