#include "CacheEntry.h"

CacheEntry::CacheEntry() : _state(INIT) {
    _data = new VectorBuffer();
}

void CacheEntry::drop() {
    delete _data;
    _data = NULL;
    _state = DROPPED;
}

void CacheEntry::cached() {
    _state = CACHED;
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

CacheEntry::CacheEntryState CacheEntry::get_state() const {
    return _state;
}
