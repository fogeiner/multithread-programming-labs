#include "Cache.h"
#include "../Client/Client.h"
#include "../../libs/Logger/Logger.h"
#include "../../libs/Buffer/VectorBuffer.h"
#include "../Retranslator/Retranslator.h"
#include "../Downloader/Downloader.h"
#include "../config.h"
// ------------CacheEntry-----------------------

void CacheEntry::set_header_end_index(int index) {
    assert(index > 0);

    Logger::debug("Index of header end: %d", index);
    _header_end_index = index;
}

CacheEntry::CacheEntry(std::string url) : _url(url), _header_end_index(-1) {
    _b = new VectorBuffer();
}

CacheEntry::~CacheEntry() {
    delete _b;
}

void CacheEntry::retranslator() {
    //
    Retranslator *r = new Retranslator(_d, _c, _b);

    delete this;
}

void CacheEntry::activate() {
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

void CacheEntry::add_client(Client *c) {
    _c.push_back(c);
}

void CacheEntry::remove_client(Client *c) {
    _c.remove(c);
}

std::string CacheEntry::url() const {
    return _url;
}

int CacheEntry::header_end_index() {
    assert(_header_end_index != -1);
    return _header_end_index;
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
        return iter->second;
    } else {
        return NULL;
    }
}

void Cache::add(std::string key, CacheEntry *ce) {
    assert(ce != NULL);
    _c.insert(std::pair<std::string, CacheEntry*>(key, ce));
}

void Cache::remove(std::string key) {
    std::map<std::string, CacheEntry*>::iterator iter = _c.find(key);
    assert(iter != _c.end());
    _c.erase(iter);
}

std::map<std::string, CacheEntry*> Cache::_c;