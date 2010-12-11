#include "Cache.h"
#include "../Client/Client.h"
#include "../../libs/Logger/Logger.h"
#include "../../libs/Buffer/VectorBuffer.h"
#include "../Retranslator/Retranslator.h"
#include "../Downloader/Downloader.h"
#include "../config.h"

CacheEntry::CacheEntry(std::string url, const Buffer *query) : _d(NULL), _url(url) {
    Logger::debug("Creating new CacheEntry");
    _b = new VectorBuffer();
    _query = new VectorBuffer();
    _query->append(query);
}

CacheEntry::~CacheEntry() {
    Logger::debug("Deleteing CacheEntry");
    delete _b;
    delete _query;
}

void CacheEntry::start_retranslator() {
    Logger::debug("Turning retranslator mode");
    new Retranslator(_d, _c, _b);
}

void CacheEntry::activate() {
    Logger::debug("Activating CacheEntry (allocating Downloader)");
    try {
        _d = new Downloader(this);
    } catch (DNSException &ex) {
        Logger::debug("Requested server not found");
        for (std::list<Client*>::iterator i = _c.begin();
                i != _c.end(); ++i) {
            Logger::debug("Deleting CacheEntry clients");
            Client *c = *i;
            c->error(ProxyConfig::server_not_found_msg);
        }
        Cache::instance()->remove(_url);
    }
}

void CacheEntry::downloader_connect_timeout(){

        Logger::debug("Connection to server timed out");
        for (std::list<Client*>::iterator i = _c.begin();
                i != _c.end(); ++i) {
            Logger::debug("Deleting CacheEntry clients");
            Client *c = *i;
            c->error(ProxyConfig::server_not_found_msg);
        }
        Cache::instance()->remove(_url);

}


void CacheEntry::add_client(Client *c) {
    _c.push_back(c);
}

void CacheEntry::remove_client(Client *c) {
    _c.remove(c);
}

std::string CacheEntry::url() const {
    return _url;
}

const Buffer *CacheEntry::get_query() {
    return this->_query;
}

Buffer *CacheEntry::get_buffer() {
    return this->_b;
}

int CacheEntry::data_size() const {
    return this->_b->size();
}

void CacheEntry::download_finished() {
    this->_d = NULL;
}

bool CacheEntry::is_download_finished() const {
    return this->_d == NULL;
}
// ----------------Cache------------------

Cache::Cache() {
}

Cache *Cache::instance() {
    static Cache c;
    return &c;
}

CacheEntry *Cache::get(std::string key) {
    std::map<std::string, CacheEntry*>::iterator iter = _c.find(key);
    if (iter != _c.end()) {
        Logger::debug("CacheEntry found in Cache");
        return iter->second;
    } else {

        Logger::debug("CacheEntry not found in Cache");
        return NULL;
    }
}

void Cache::add(std::string key, CacheEntry *ce) {
    Logger::debug("Adding new CacheEntry");
    assert(ce != NULL);
    _c.insert(std::pair<std::string, CacheEntry*>(key, ce));
}

void Cache::remove(std::string key) {
    Logger::debug("Removing CacheEntry from Cache");
    std::map<std::string, CacheEntry*>::iterator iter = _c.find(key);
    if (iter != _c.end())
        _c.erase(iter);
}

std::map<std::string, CacheEntry*> Cache::_c;
