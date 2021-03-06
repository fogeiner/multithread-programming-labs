#include "CacheEntry.h"
#include "Cache.h"

CacheEntry::CacheEntry() : _state(INIT) {
    _data = new VectorBuffer();
}

CacheEntry::CacheEntry(const CacheEntry &orig) {
    _data = new VectorBuffer();
    _data->append(orig._data);
    _state = orig._state;
}

CacheEntry &CacheEntry::operator=(const CacheEntry &orig) {
    if(this == &orig){
        return *this;
    }

    _data = new VectorBuffer();
    _data->append(orig._data);
    _state = orig._state;
    return *this;
}

CacheEntry::~CacheEntry() {
    delete _data;
}

void CacheEntry::cached() {
    Cache::_cache_mutex.lock();
    if(_state != CACHED){
        Cache::_size += data()->size();
    }
    _state = CACHED;
    Cache::_cache_mutex.unlock();
}

void CacheEntry::caching() {
    Cache::_cache_mutex.lock();
    _state = CACHING;
    Cache::_cache_mutex.unlock();
}

int CacheEntry::size() const {
    return _data->size();
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
