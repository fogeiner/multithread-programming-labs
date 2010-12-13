#pragma once
#include "CacheEntry.h"
#include "../BrokenUpHTTPRequest.h"
#include "../../libs/Mutex/Mutex.h"
#include <string>
#include <map>
#include <list>

class Cache {
private:
    static std::map<std::string, CacheEntry> _cache;
    static std::map<std::string, std::list<DownloadListener*> > _listners;
    static Mutex _mutex;
    static Cache *instance();
    Cache();
public:
    static void request(BrokenUpHTTPRequest request, DownloadListener *download_listener);

    // Downloader adds recv'ed info
    // should add to buffer and given it also to DownloadListeners
    static void add_data(std::string key, const Buffer *b);

    // Downloader successfully finished download
    // should mark CacheEntry as finished and keep it
    static void finished(std::string key);

    // Downloader failed to connect to server
    // Send 504 error Gateway timeout to clients
    // and remove entry
    static void connect_failed(std::string key);

    // Downloader failed during send; it means 
    // there's nothing to be sent to clients that's why
    // Send 500 error Internal error to clients
    // and remove entry
    static void send_failed(std::string key);

    // Downloader failed during recv (ECONNREFUSED)
    // Send whatever there's downloaded in cache
    // and remove entry
    static void recv_failed(std::string key);
};