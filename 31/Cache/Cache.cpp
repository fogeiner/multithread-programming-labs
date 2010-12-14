#include "Cache.h"
#include <cassert>
#include "../../libs/Logger/Logger.h"

const int Cache::MAX_CACHE_ENTRY_SIZE = ProxyConfig::max_cache_entry_size;
const int Cache::MAX_CACHE_SIZE = ProxyConfig::max_cache_size;

std::map<std::string, CacheEntry> Cache::_cache;
std::map<std::string, std::list<ClientListener*> > Cache::_listeners;

const std::string Cache::HTTP_NOT_IMPLEMENTED("HTTP_NOT_IMPLEMENTED");
const std::string Cache::HTTP_BAD_REQUEST("HTTP_BAD_REQUEST");
const std::string Cache::HTTP_INTERNAL_ERROR("HTTP_INTERNAL_ERROR");
const std::string Cache::HTTP_SERVICE_UNAVAILABLE("HTTP_SERVICE_UNAVAILABLE");

Mutex Cache::_mutex(Mutex::ERRORCHECK_MUTEX);

void Cache::init() {
    // adding messages with errors
    _mutex.lock();
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
    _mutex.unlock();
}

void Cache::client_request(std::string key, ClientListener *download_listener, BrokenUpHTTPRequest *request) {
    _mutex.lock();

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
    _mutex.unlock();
}

void Cache::client_finished(std::string key, ClientListener *download_listener) {
    _mutex.lock();
    assert(_cache.find(key) != _cache.end());
    _listeners[key].remove(download_listener);
    _mutex.unlock();
}

std::string Cache::downloader_add_data(std::string key, const Buffer *b) {
    _mutex.lock();
    assert(_cache.find(key) != _cache.end());

    _mutex.unlock();
}

void Cache::downloader_finished(std::string key) {
}

void Cache::downloader_connect_failed(std::string key) {
}

void Cache::downloader_send_failed(std::string key) {
}

void Cache::downloader_recv_failed(std::string key) {
}