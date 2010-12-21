#include "CacheEntry.h"
#include <algorithm>
#include "Client.h"

CacheEntry::CacheEntry(BrokenUpHTTPRequest request) :
_state(CacheEntry::CACHING),
_data(new VectorBuffer()),
_bytes_received(0),
_request(request),
_mutex(Mutex::ERRORCHECK_MUTEX) {
}

CacheEntry::~CacheEntry() {
    delete _data;
}

CacheEntry::CacheEntryState CacheEntry::get_state() const {
    return _state;
}

void CacheEntry::set_state(CacheEntryState state) {
    _state = state;
}

int CacheEntry::add_data(const char *msg) {
    Logger::debug("CacheEntry::add_data()");
    int clients_count;

    clients_count = _clients.size();
    _data->append(msg);
    _bytes_received += _data->size();


    return clients_count;
}

const Buffer *CacheEntry::data() const {
    return _data;
}

int CacheEntry::add_data(const Buffer *buffer) {
    Logger::debug("CacheEntry::add_data()");

    int clients_count;

    clients_count = _clients.size();
    _data->append(buffer);
    _bytes_received += buffer->size();

    return clients_count;
}

void CacheEntry::add_client(Client *client) {
    Logger::debug("CacheEntry::add_client()");
    client->set_cache_entry(this);
    _clients_bytes_got[client] = 0;
    _clients.push_back(client);
}

;

struct CompareClientsBytes {

    bool operator()(const std::pair<Client*, int>& left, const std::pair<Client*, int>& right) const {
        return left.second < right.second;
    }
};

void CacheEntry::data_got(int bytes_got, Client* client) {
    Logger::debug("CacheEntry::data_got()");
    _clients_bytes_got[client] = bytes_got;
    if (_state == DOWNLOADING || _state == FINISHED) {
        int min = (*std::min_element(_clients_bytes_got.begin(), _clients_bytes_got.end(), CompareClientsBytes())).second;
        _data->drop_first(min - (_bytes_received - _data->size()));
    }
}

void CacheEntry::remove_client(Client *client) {
    Logger::debug("CacheEntry::remove_client()");
    bool to_delete = false;

    _clients.remove(client);
    _clients_bytes_got.erase(client);

    to_delete = (_clients.size() == 0) && (_downloader == NULL) && (_state != CACHED);

    if (to_delete) {
        delete this;
    }
}

void CacheEntry::remove_downloader() {
    Logger::debug("CacheEntry::remove_downloader()");
    bool to_delete = false;

    _downloader = NULL;
    to_delete = (_clients.size() == 0) && (_downloader == NULL) && (_state != CACHED);

    if (to_delete) {
        delete this;
    }
}

void CacheEntry::set_downloader(Downloader *downloader) {
    Logger::debug("CacheEntry::set_downloader()");
    _downloader = downloader;
}

int CacheEntry::bytes_received() const {
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
