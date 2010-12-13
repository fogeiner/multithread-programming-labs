#pragma once
#include "CacheEntry.h"
#include "../BrokenUpHTTPRequest.h"
#include <string>
#include <map>
#include <list>

class Cache{
private:
    static std::map<std::string, CacheEntry> _cache;
    static std::map<std::string, std::list<DownloadListener*> > _listners;
    static Cache *instance();
    Cache();
public:
    static void request(BrokenUpHTTPRequest request, DownloadListener *download_listener);
    
};