#pragma once

#include <list>

#include "../libs/Logger/Logger.h"
#include "../libs/Mutex/Mutex.h"
#include "../libs/CondVar/CondVar.h"
#include "../libs/Buffer/VectorBuffer.h"


class Client;
class Downloader;
class CacheEntry {
public:

    enum CacheEntryStatus {
        CONNECTION_ERROR, SEND_ERROR, RECV_ERROR, CACHING, CACHED, STOPPED
    };
private:
    std::list<Client*> _clients;
    Downloader *_downloader;
    CacheEntryStatus _state;
    Buffer *_data;
    mutable Mutex _mutex;
    mutable CondVar _cv;
public:
    CacheEntry();
    ~CacheEntry();
    CacheEntryStatus get_state() const;
    void set_state(CacheEntryStatus state);
    int add_data(const char *msg);
    int add_data(const Buffer *buffer);
    void add_client(Client *client);
    void remove_client(Client *client);
    void set_downloader(Downloader *downloader);
    void remove_downloader();
    int size() const;
};
