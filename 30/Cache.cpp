#include "Cache.h"
#include "Downloader.h"
#include "CacheEntry.h"
#include "config.h"
std::map<std::string, CacheEntry*> Cache::_cache;
Mutex Cache::_cache_mutex(Mutex::RECURSIVE_MUTEX);
size_t Cache::_size;
Mutex Cache::_size_mutex(Mutex::RECURSIVE_MUTEX);
const size_t Cache::MAX_CACHE_ENTRY_SIZE = ProxyConfig::max_cache_entry_size;
const size_t Cache::MAX_CACHE_SIZE = ProxyConfig::max_cache_size;

// kinda bad request but that cannot be processed
const std::string Cache::HTTP_NOT_IMPLEMENTED("HTTP_NOT_IMPLEMENTED");
// bad request
const std::string Cache::HTTP_BAD_REQUEST("HTTP_BAD_REQUEST");
// send/recv failed
const std::string Cache::HTTP_INTERNAL_ERROR("HTTP_INTERNAL_ERROR");
// couldn't connect
const std::string Cache::HTTP_SERVICE_UNAVAILABLE("HTTP_SERVICE_UNAVAILABLE");

void Cache::init() {
    Logger::debug("Cache::init()");

    Cache::_size = 0;
    // adding messages with errors
    _cache[HTTP_NOT_IMPLEMENTED] = new CacheEntry();
    _cache[HTTP_NOT_IMPLEMENTED]->add_data("HTTP/1.0 501 Not Implemented\r\n\r\n<html><head><title>501</title></head><body><h1>501 Not Implemented</h1><hr>Proxy</body></html>");
    _cache[HTTP_NOT_IMPLEMENTED]->set_state(CacheEntry::CACHED);

    _cache[HTTP_BAD_REQUEST] = new CacheEntry();
    _cache[HTTP_BAD_REQUEST]->add_data("HTTP/1.0 400 Bad Request\r\n\r\n<html><head><title>400</title></head><body><h1>400 Bad Request</h1><hr>Proxy</body></html>");
    _cache[HTTP_BAD_REQUEST]->set_state(CacheEntry::CACHED);

    _cache[HTTP_INTERNAL_ERROR] = new CacheEntry();
    _cache[HTTP_INTERNAL_ERROR]->add_data("HTTP/1.0 500 Internal Error\r\n\r\n<html><head><title>500</title></head><body><h1>500 Internal Error</h1><hr>Proxy</body></html>");
    _cache[HTTP_INTERNAL_ERROR]->set_state(CacheEntry::CACHED);

    _cache[HTTP_SERVICE_UNAVAILABLE] = new CacheEntry();
    _cache[HTTP_SERVICE_UNAVAILABLE]->add_data("HTTP/1.0 503 Service Unavailable\r\n\r\n<html><head><title>503</title></head><body><h1>503 Service Unavailable</h1><hr>Proxy</body></html>");
    _cache[HTTP_SERVICE_UNAVAILABLE]->set_state(CacheEntry::CACHED);
}

size_t Cache::size() {
    return Cache::_size;
}

void Cache::bytes_added(size_t bytes) {
    Cache::_size_mutex.lock();
    Cache::_size += bytes;
    Cache::_size_mutex.unlock();
}

void Cache::bytes_removed(size_t bytes) {
    Cache::_size_mutex.lock();
    Cache::_size -= bytes;
    Cache::_size_mutex.unlock();
}

void Cache::request(std::string url, Client *client) {
    return Cache::request(BrokenUpHTTPRequest(url), client);
}

void Cache::request(BrokenUpHTTPRequest request, Client *client) {
    Logger::debug("Cache::request(%s, %p)", request.url().c_str(), client);
    const std::string key = request.url();

    Cache::_cache_mutex.lock();

    // if such entry is already present
    if (_cache.find(key) != _cache.end()) {
        CacheEntry *ce = _cache[key];

        if (ce->get_state() == CacheEntry::CACHED) {
            Logger::info("Cache HIT %s", key.c_str());
        } else if (ce->get_state() == CacheEntry::CACHING) {
            Logger::info("Cache CACHING %s", key.c_str());
        } else {
            assert(false);
        }

        ce->add_client(client);
        // if there's no such entry we should create one
    } else {
        Logger::info("Cache NEW %s", key.c_str());
        CacheEntry *ce = new CacheEntry(request);
        ce->add_client(client);
		Downloader *downloader = NULL;

        try {
            Downloader *downloader = new Downloader(ce);
            Thread downloader_thread(Downloader::run, downloader);
            downloader_thread.run();
            downloader_thread.detach();
            _cache[key] = ce;
        } catch (ThreadException &ex) {
            Logger::error("Cache::request() ThreadException: %s", ex.what());
            delete ce;
			delete downloader;
            _cache[HTTP_INTERNAL_ERROR]->add_client(client);
        } catch (SocketException &ex){
            Logger::error("Cache::request() SocketException: %s", ex.what());
            delete ce;
            _cache[HTTP_INTERNAL_ERROR]->add_client(client);
		}
    }

    Cache::_cache_mutex.unlock();
}

void Cache::drop(std::string url) {
    Logger::debug("Cache::drop(%s)", url.c_str());
    Cache::_cache_mutex.lock();
    CacheEntry *ce = _cache[url];
    Cache::bytes_removed(ce->bytes_received());
    _cache.erase(url);
    Cache::_cache_mutex.unlock();
}
