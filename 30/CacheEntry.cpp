#include "CacheEntry.h"

CacheEntry::CacheEntry() :
_state(CacheEntry::CACHING),
_data(new VectorBuffer()),
_mutex(Mutex::ERRORCHECK_MUTEX) {
}

CacheEntry::~CacheEntry() {
    delete _data;
}

CacheEntry::CacheEntryStatus CacheEntry::get_state() const {
    return _state;
}

void CacheEntry::set_state(CacheEntryStatus state) {
    _mutex.lock();
    _state = state;
    _cv.broadcast();
    _mutex.unlock();
}

int CacheEntry::add_data(const char *msg) {
    int clients_count;

    _mutex.lock();
    clients_count = _clients.size();
    _data->append(msg);
    _cv.broadcast();
    _mutex.unlock();

    return clients_count;
}

int CacheEntry::add_data(const Buffer *buffer) {
    int clients_count;

    _mutex.lock();
    clients_count = _clients.size();
    _data->append(buffer);
    _cv.broadcast();
    _mutex.unlock();

    return clients_count;
}

void CacheEntry::add_client(Client *client) {
    _mutex.lock();
    _clients.push_back(client);
    _cv.broadcast();
    _mutex.unlock();
}

void CacheEntry::remove_client(Client *client) {
    bool to_delete = false;

    _mutex.lock();
    _clients.remove(client);
    to_delete = (_clients.size() == 0) && (_downloader == NULL) && (_state != CACHED);
    _cv.broadcast();
    _mutex.unlock();

    if (to_delete) {
        delete this;
    }
}

void CacheEntry::remove_downloader() {
    bool to_delete = false;

    _mutex.lock();
    _downloader = NULL;
    to_delete = (_clients.size() == 0) && (_downloader == NULL) && (_state != CACHED);
    _cv.broadcast();
    _mutex.unlock();

    if (to_delete) {
        delete this;
    }
}

void CacheEntry::set_downloader(Downloader *downloader) {
    _mutex.lock();
    _downloader = downloader;
    _cv.broadcast();
    _mutex.unlock();
}

int CacheEntry::size() const {
    int size;
    _mutex.lock();
    size = _data->size();
    _cv.broadcast();
    _mutex.unlock();
    return size;
}
