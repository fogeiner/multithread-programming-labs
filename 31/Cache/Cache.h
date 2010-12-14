#pragma once
#include "CacheEntry.h"
#include "../ClientListener.h"
#include "../config.h"
#include "../BrokenUpHTTPRequest.h"
#include "../../libs/Mutex/Mutex.h"
#include <string>
#include <map>
#include <list>

class Cache {
private:
    static std::map<std::string, CacheEntry> _cache;
    static std::map<std::string, std::list<ClientListener*> > _listeners;

    static const int MAX_CACHE_SIZE;
    static const int MAX_CACHE_ENTRY_SIZE;
    static Mutex _mutex;

    Cache();
public:

    static const std::string HTTP_NOT_IMPLEMENTED;
    static const std::string HTTP_BAD_REQUEST;
    static const std::string HTTP_INTERNAL_ERROR;
    static const std::string HTTP_SERVICE_UNAVAILABLE;

    static void init();

    static Cache *instance();

    // in case Entry is present in cache we just add whatever is ready to client's buffer
    // and add it in _listeners list for the given cache entry
    // otherwise create new cache entry, new downloader and subscribe client to it

    // in case cache size exceeds maximum cache size
    // new requests create cache entries that work only in non-caching mode

    static void client_request(std::string key, ClientListener *download_listener, BrokenUpHTTPRequest *request = NULL);

    // in case during the send to client send failed
    // we gonna remove client from cache
    static void client_finished(std::string key, ClientListener *download_listener);

    // Downloader adds recv'ed info
    // should add to buffer and given it also to DownloadListeners

    // in case after the operation cache entry exceeds maximum size
    // cache entry is set in non-caching mode with the
    // key alteration
    // each client gets it's own cache entry with unique cache
    // key can be composed out of number of client in listeners list,
    // current time and original url

    // return new key (or the old one)
    static std::string downloader_add_data(std::string key, const Buffer *b);

    // Downloader successfully finished download
    // should mark CacheEntry as finished and keep it
    static void downloader_finished(std::string key);

    // Downloader failed to connect to server
    // Send 504 error Gateway timeout to clients
    // and remove entry
    static void downloader_connect_failed(std::string key);

    // Downloader failed during send; it means 
    // there's nothing to be sent to clients that's why
    // Send 500 error Internal error to clients
    // and remove entry
    static void downloader_send_failed(std::string key);

    // Downloader failed during recv (ECONNREFUSED)
    // Send whatever there's downloaded in cache
    // and remove entry
    static void downloader_recv_failed(std::string key);
};