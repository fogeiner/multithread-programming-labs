#pragma once
#include <list>

class CacheEntry{
private:
    std::list<DownloadListener*> _listeners;
public:
};