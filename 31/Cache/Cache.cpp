#include "Cache.h"

const int Cache::MAX_CACHE_ENTRY_SIZE = ProxyConfig::max_cache_entry_size;
const int Cache::MAX_CACHE_SIZE = ProxyConfig::max_cache_size;

std::map<std::string, CacheEntry> Cache::_cache;
std::map<std::string, std::list<DownloadListener*> > Cache::_listeners;

const std::string Cache::HTTP_NOT_IMPLEMENTED("HTTP_NOT_IMPLEMENTED");
const std::string Cache::HTTP_BAD_REQUEST("HTTP_BAD_REQUEST");
const std::string Cache::HTTP_INTERNAL_ERROR("HTTP_INTERNAL_ERROR");

Mutex Cache::_mutex(Mutex::ERRORCHECK_MUTEX);

Cache::Cache() {
    // adding messages with errors

}

void Cache::client_request(std::string key, DownloadListener *download_listener, BrokenUpHTTPRequest *request) {
    _mutex.lock();
    _mutex.unlock();
}
void Cache::client_finished(std::string key, DownloadListener *download_listener);
std::string Cache::downloader_add_data(std::string key, const Buffer *b);
void Cache::downloader_finished(std::string key);
void Cache::downloader_connect_failed(std::string key);
void Cache::downloader_send_failed(std::string key);
void Cache::downloader_recv_failed(std::string key);