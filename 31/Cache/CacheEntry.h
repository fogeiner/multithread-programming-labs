#pragma once
#include "../DownloadListener.h"
#include "../../libs/Buffer/VectorBuffer.h"
#include <list>

class CacheEntry  {
private:
    std::list<DownloadListener*> _listeners;
    Buffer *_data;

    bool _cached;

    bool _finished;
    bool _cancelled;
public:
    CacheEntry();

};