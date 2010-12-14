#pragma once
#include "../DownloadListener.h"
#include "../../libs/Buffer/VectorBuffer.h"
#include <list>

class CacheEntry {
private:
    Buffer *_data;

    // response was 200, download finished ok; size is not more than MAX_CACHE_ENTRY_SIZE
    bool _cached;

    // flag that means that CacheEntry was set to retranslator mode
    bool _dropped;

public:
    CacheEntry();
    void drop();
    void cached();
    bool is_cached() const;
    bool is_dropped() const;
    void add_data(const Buffer *b);
    void add_data(const char *msg);
    const Buffer *data() const;
};