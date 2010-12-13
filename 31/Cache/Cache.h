#pragma once
#include "CacheEntry.h"
#include <map>

class Cache{
private:
    static std::map<std::string, CacheEntry> _cache;
    static Cache *instance();
    Cache();
public:
    static void request(DownloadListener *download_listener);
};