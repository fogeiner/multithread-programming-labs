#include "Cache.h"
#include <cassert>
#include "../../libs/Logger/Logger.h"

const int Cache::MAX_CACHE_ENTRY_SIZE = ProxyConfig::max_cache_entry_size;
const int Cache::MAX_CACHE_SIZE = ProxyConfig::max_cache_size;

std::map<std::string, CacheEntry> Cache::_cache;

std::map<std::string, Retranslator*> Cache::_retranslators;

int Cache::_size;

// kinda bad request but that cannot be processed
const std::string Cache::HTTP_NOT_IMPLEMENTED("HTTP_NOT_IMPLEMENTED");
// bad request
const std::string Cache::HTTP_BAD_REQUEST("HTTP_BAD_REQUEST");
// send/recv failed
const std::string Cache::HTTP_INTERNAL_ERROR("HTTP_INTERNAL_ERROR");
// couldn't connect
const std::string Cache::HTTP_SERVICE_UNAVAILABLE("HTTP_SERVICE_UNAVAILABLE");

Mutex Cache::_mutex(Mutex::ERRORCHECK_MUTEX);

void Cache::init() {
    Cache::_size = 0;
    // adding messages with errors
    _cache[HTTP_NOT_IMPLEMENTED] = CacheEntry();
    _cache[HTTP_NOT_IMPLEMENTED].add_data("HTTP/1.0 501 Not Implemented\r\n\r\n<html><head><title>501</title></head><body><h1>501 Not Implemented</h1><hr>Proxy</body></html>");
    _cache[HTTP_NOT_IMPLEMENTED].cached();

    _cache[HTTP_BAD_REQUEST] = CacheEntry();
    _cache[HTTP_BAD_REQUEST].add_data("HTTP/1.0 400 Bad Request\r\n\r\n<html><head><title>400</title></head><body><h1>400 Bad Request</h1><hr>Proxy</body></html>");
    _cache[HTTP_BAD_REQUEST].cached();

    _cache[HTTP_INTERNAL_ERROR] = CacheEntry();
    _cache[HTTP_INTERNAL_ERROR].add_data("HTTP/1.0 500 Internal Error\r\n\r\n<html><head><title>500</title></head><body><h1>500 Internal Error</h1><hr>Proxy</body></html>");
    _cache[HTTP_INTERNAL_ERROR].cached();

    _cache[HTTP_SERVICE_UNAVAILABLE] = CacheEntry();
    _cache[HTTP_SERVICE_UNAVAILABLE].add_data("HTTP/1.0 503 Service Unavailable\r\n\r\n<html><head><title>503</title></head><body><h1>503 Service Unavailable</h1><hr>Proxy</body></html>");
    _cache[HTTP_SERVICE_UNAVAILABLE].cached();
}

ClientRetranslator *Cache::request(std::string url, ClientListener *client_listener) {
    
    return Cache::request(BrokenUpHTTPRequest(url), client_listener);
}

ClientRetranslator *Cache::request(BrokenUpHTTPRequest request, ClientListener *client_listener) {
    Logger::debug("Cache::request(%s)", request.url.c_str());

    //   _mutex.lock();
    std::string key = request.url;
    ClientRetranslator *return_retranslator;

    // if such entry is already present
    if (_cache.find(key) != _cache.end()) {
        CacheEntry ce = _cache[key];
        // if CacheEntry is CACHED append CacheEntry's buffer
        // to client buffer and setting finished flag;
        if (ce.get_state() == CacheEntry::CACHED) {
            Logger::info("Cache HIT %s", key.c_str());
            client_listener->add_data(ce.data());
            client_listener->finished(true);
            return_retranslator = NULL;
            
            // if it's CACHING we can add that client to
            // list of clients
        } else if (ce.get_state() == CacheEntry::CACHING) {
            Logger::info("Cache CACHING %s", key.c_str());
            client_listener->add_data(ce.data());
            _retranslators[key]->add_client(client_listener);
            return_retranslator = _retranslators[key];

            // that is meant to never happen
        } else {
            assert(false);
        }

        // if there's no such entry we should create one
    } else {
        Logger::info("Cache NEW %s", key.c_str());
        _cache[key] = CacheEntry();
        _retranslators[key] = new Retranslator(request, _cache[key], client_listener);

        return_retranslator = _retranslators[key];
    }
    //    _mutex.unlock();
    return return_retranslator;
}

void Cache::drop(std::string key) {
    //   _mutex.lock();
    Logger::info("Cache DROP %s", key.c_str());
    Cache::_size -= _cache[key].size();
    _cache.erase(key);
    _retranslators.erase(key);
    //  _mutex.unlock();
}

void Cache::bytes_added(int bytes){
    Cache::_size += bytes;
}

int Cache::size(){
    return Cache::_size;
}