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

    r->_mutex.lock();
    client_listener->add_data(r->_ce.data());
    r->_clients.push_back(client_listener);
    r->_mutex.unlock();
}

void CacheRetranslator::client_finished(Retranslator *r, ClientListener *client_listener) {
    Logger::debug("CacheRetranslator::client_finished() start");
    r->delete_client(client_listener);
    Logger::debug("CacheRetranslator::client_finished() end");
}

void CacheRetranslator::download_add_data(Retranslator *r, const Buffer *b) {
    Logger::debug("CacheRetranslator::download_add_data()");
    r->_mutex.lock();
    if (r->clients_count() == 0) {
            Logger::debug("CacheRetranslator::download_add_data() no clients; cancelling download");

        r->_download_listener->cancel();

        if (r->_ce.get_state() != CacheEntry::CACHED) {
            Cache::drop(r->_request.url);
        }

        r->_mutex.unlock();
        delete r;
        return;
    }
    // add to CacheEntry
    r->_ce.add_data(b);

    // add to clients
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->add_data(b);
    }

    if (Cache::size() > Cache::MAX_CACHE_SIZE) {
        Logger::debug("CacheRetranslator::download_add_data() Switching to DirectRetranslator due to general cache overflow");
        Logger::info("Dropping: cache max size exceeded");
        Cache::drop(r->_request.url);
        r->change_state(DirectRetranslator::instance());
        r->_mutex.unlock();
        return;
    }

    // strlen("HTTP/1.x 200") == 12
    if ((r->_response_code_received == false) && (r->_ce.size() >= 12)) {
        Logger::debug("CacheRetranslator::download_add_data() Analyzing response");
        r->_response_code_received = true;

        std::string response = *r->_ce.data();
        std::string word1;
        std::string word2;
        std::istringstream iss(response, std::istringstream::in);
        iss >> word1;
        iss >> word2;

        if ((word1 == "HTTP/1.0" || word1 == "HTTP/1.1") && word2 == "200") {
            Logger::debug("CacheRetranslator::download_add_data() Valid 200 response, caching");
        } else {
            Logger::debug("CacheRetranslator::download_add_data() Switching to DirectRetranslator due to non 200 response");
            Logger::info("Dropping: code %s", word2.c_str());
            Cache::drop(r->_request.url);
            r->change_state(DirectRetranslator::instance());
            r->_mutex.unlock();
            return;
        }
    }

    if (r->_ce.size() > ProxyConfig::max_cache_entry_size) {
        Logger::debug("CacheRetranslator::download_add_data() Switching to DirectRetranslator due to CacheEntry overflow");
        Logger::info("Dropping: cache entry max size exceeded");
        Cache::drop(r->_request.url);
        r->change_state(DirectRetranslator::instance());
        r->_mutex.unlock();
        return;
    }

    r->_mutex.unlock();
}

void CacheRetranslator::download_finished(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_finished()");
    r->_mutex.lock();
    r->_ce.cached();
    Logger::info("Cache DONE %s", r->_request.url.c_str());
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->finished();
    }
    r->_mutex.unlock();
    delete r;
}

void CacheRetranslator::download_connect_failed(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_connect_failed()");

    Logger::info("Dropping: connect failed");
    r->_mutex.lock();
    Cache::drop(r->_request.url);

    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {

        Cache::request(Cache::HTTP_SERVICE_UNAVAILABLE, *i);
    }
    r->_mutex.unlock();
    delete r;
}

void CacheRetranslator::download_send_failed(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_send_failed()");

    Logger::info("Dropping: send failed");
    r->_mutex.lock();
    Cache::drop(r->_request.url);
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {

        Cache::request(Cache::HTTP_INTERNAL_ERROR, *i);

    }
    r->_mutex.unlock();
    delete r;
}

void CacheRetranslator::download_recv_failed(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_recv_failed()");

    Logger::info("Dropping: recv failed");
    r->_mutex.lock();
    Cache::drop(r->_request.url);
    for (std::list<ClientListener*>::iterator i = r->_clients.begin();
            i != r->_clients.end(); ++i) {
        (*i)->finished();
    }
    r->_mutex.unlock();
    delete r;
}
