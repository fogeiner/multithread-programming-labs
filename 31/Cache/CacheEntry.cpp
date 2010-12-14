#include "CacheEntry.h"

CacheEntry::CacheEntry(): _cached(false), _dropped(false) {
    _data = new VectorBuffer();
}

void CacheEntry::drop() {
    delete _data;
    _data = NULL;
    _dropped = true;
}

void CacheEntry::cached() {
    _cached = true;
}

bool CacheEntry::is_cached() const {
    return _cached;
}

bool CacheEntry::is_dropped() const {
    return _dropped;
}

void CacheEntry::add_data(const Buffer *b) {
    _data ->append(b);
}

void CacheEntry::add_data(const char *msg) {
    _data->append(msg);
}

const Buffer *CacheEntry::data() const {
    return _data;
}