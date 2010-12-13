#pragma once
#include "../DownloadListener.h"
#include "../../libs/Buffer/VectorBuffer.h"
#include <list>

class CacheEntry  {
private:
    Buffer *_data;

    // response was 200, download finished ok; size is not more than MAX_CACHE_ENTRY_SIZE
    bool _cached;

    // download finished; not data will be added
    bool _finished;
    
    // 
    bool f;

public:
    CacheEntry();

};