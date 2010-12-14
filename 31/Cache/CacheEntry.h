#pragma once
#include "../DownloadListener.h"
#include "../../libs/Buffer/VectorBuffer.h"
#include <list>

class CacheEntry {
public:

    enum CacheEntryState {
        INIT, CACHING, CACHED
    };

private:
    Buffer *_data;
    CacheEntryState _state;
public:
    CacheEntry();
    CacheEntry(const CacheEntry &orig);
    CacheEntry &operator=(const CacheEntry &orig);
    ~CacheEntry();
    void drop();
    void cached();
    void caching();
    int size() const;
    void add_data(const Buffer *b);
    void add_data(const char *msg);
    const Buffer *data() const;

    CacheEntryState get_state() const;
};