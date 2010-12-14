#pragma once
#include "../DownloadListener.h"
#include "../../libs/Buffer/VectorBuffer.h"
#include <list>

class CacheEntry {
public:

    enum CacheEntryState {
        INIT, CACHING, CACHED, DROPPED
    };

private:
    Buffer *_data;
    CacheEntryState _state;
public:
    CacheEntry();
    void drop();
    void cached();
    bool is_cached() const;
    bool is_dropped() const;
    void add_data(const Buffer *b);
    void add_data(const char *msg);
    const Buffer *data() const;

    CacheEntryState get_state() const;
};