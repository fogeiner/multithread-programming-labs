#pragma once

#include <string>
#include "../libs/Logger/Logger.h"
#include "../libs/Mutex/Mutex.h"
#include "../libs/Logger/Logger.h"
#include "../libs/Thread/Thread.h"
#include "BrokenUpHTTPRequest.h"
#include "CacheEntry.h"
#include <map>

class Cache {
private:
    static std::map<std::string, CacheEntry*> _cache;
    static Mutex _cache_mutex;
    static Mutex _size_mutex;
    static size_t _size;
    Cache();
public:
    static const size_t MAX_CACHE_SIZE;
    static const size_t MAX_CACHE_ENTRY_SIZE;

    static const std::string HTTP_NOT_IMPLEMENTED;
    static const std::string HTTP_BAD_REQUEST;
    static const std::string HTTP_INTERNAL_ERROR;
    static const std::string HTTP_SERVICE_UNAVAILABLE;

    static void init();
    static void request(BrokenUpHTTPRequest request, Client *client);
    static void request(std::string url, Client *client);
    static size_t size();

    static void bytes_added(size_t bytes);
    static void bytes_removed(size_t bytes);
    static void drop(std::string key);
};

