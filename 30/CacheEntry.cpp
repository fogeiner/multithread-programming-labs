#include <algorithm>
#include "CacheEntry.h"
#include "Client.h"
#include "Downloader.h"

CacheEntry::CacheEntry(BrokenUpHTTPRequest request) :
_downloader(Downloader::dummy_downloader()),
_state(CacheEntry::CACHING),
_data(new VectorBuffer()),
_bytes_received(0),
_request(request),
_mutex(Mutex::ERRORCHECK_MUTEX) {
    Logger::debug("CacheEntry::CacheEntry()");
}

CacheEntry::~CacheEntry() {
    Logger::debug("CacheEntry::~CacheEntry()");
    delete _data;
}

CacheEntry::CacheEntryState CacheEntry::get_state() const {
    return _state;
}

void CacheEntry::set_state(CacheEntryState state) {
    _state = state;
}

void CacheEntry::add_data(const char *msg) {
    Logger::debug("CacheEntry::add_data()");

    _data->append(msg);
    _bytes_received += _data->size();
}

const Buffer *CacheEntry::data() const {
    return _data;
}

void CacheEntry::add_data(const Buffer *buffer) {
    Logger::debug("CacheEntry::add_data()");

    if (_state == CACHING) {
        Cache::bytes_added(buffer->size());
    }
    _data->append(buffer);
    _bytes_received += buffer->size();
}

void CacheEntry::add_client(Client *client) {
    Logger::debug("CacheEntry::add_client()");
    client->set_cache_entry(this);
    _clients_bytes_got[client] = 0;
    _clients.push_back(client);
}

struct CompareClientsBytes {

    bool operator()(const std::pair<Client*, size_t>& left, const std::pair<Client*, size_t>& right) const {
        return left.second < right.second;
    }
};

void CacheEntry::data_got(size_t bytes_got, Client* client) {
    Logger::debug("CacheEntry::data_got()");
    _clients_bytes_got[client] = bytes_got;

    if (_state == DOWNLOADING || _state == FINISHED) {
        size_t min = (*std::min_element(_clients_bytes_got.begin(),
                _clients_bytes_got.end(), CompareClientsBytes())).second;
        Logger::debug("Dropping %d bytes from CacheEntry", min - (_bytes_received - _data->size()));
        _data->drop_first(min - (_bytes_received - _data->size()));
        Cache::bytes_removed(min - (_bytes_received - _data->size()));
    }
}

bool CacheEntry::to_delete() const {
    return (_clients.size() == 0) && (_downloader == NULL) && (_state != CACHED);
}

void CacheEntry::remove_client(Client *client) {
    Logger::debug("CacheEntry::remove_client()");

    _clients.remove(client);
    _clients_bytes_got.erase(client);
}

void CacheEntry::remove_downloader() {
    Logger::debug("CacheEntry::remove_downloader()");
    _downloader = NULL;
}

void CacheEntry::set_downloader(Downloader *downloader) {
    Logger::debug("CacheEntry::set_downloader()");
    _downloader = downloader;
}

size_t CacheEntry::bytes_received() const {
    return _bytes_received;
}

const BrokenUpHTTPRequest &CacheEntry::request() const {
    return _request;
}

void CacheEntry::lock() {
    Logger::debug("CacheEntry::lock()");
    _mutex.lock();
}

void CacheEntry::unlock() {
    Logger::debug("CacheEntry::unlock()");
    _mutex.unlock();
}

void CacheEntry::wait() {
    Logger::debug("CacheEntry::wait()");
    _cv.wait(_mutex);
}

void CacheEntry::signal() {
    Logger::debug("CacheEntry::signal()");
    _cv.signal();
}

void CacheEntry::broadcast() {
    Logger::debug("CacheEntry::broadcast()");
    _cv.broadcast();
}

int CacheEntry::clients_count() const {
    return _clients.size();
}
