#include "Cache.h"
#include <cassert>
#include "../../libs/Logger/Logger.h"

const int Cache::MAX_CACHE_ENTRY_SIZE = ProxyConfig::max_cache_entry_size;
const int Cache::MAX_CACHE_SIZE = ProxyConfig::max_cache_size;

std::map<std::string, CacheEntry> Cache::_cache;

std::map<std::string, Retranslator*> Cache::_retranslators;

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

/*

    // such entry is present
    if (_cache.find(key) != _cache.end()) {
        CacheEntry ce = _cache[key];

        assert(ce.is_dropped() == false);

        // append data to client and set finished flag
        if (ce.is_cached()) {
            download_listener->add_data(key, ce.data());
            download_listener->finished();
        } else {
            // append data to client and subscribe it to the new data
            download_listener->add_data(key, ce.data());
            _listeners[key].push_back(download_listener);
        }
    } else {
        // such entry is not present
        assert(false);
    }

 */