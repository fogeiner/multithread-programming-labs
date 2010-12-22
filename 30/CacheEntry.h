#pragma once

#include <map>
#include <list>

#include "../libs/Logger/Logger.h"
#include "../libs/Mutex/Mutex.h"
#include "../libs/CondVar/CondVar.h"
#include "../libs/Buffer/VectorBuffer.h"
#include "BrokenUpHTTPRequest.h"


class Client;
class Downloader;

class CacheEntry {
public:
    // CONNECTION_ERROR -- connect failed,
    // SEND_ERROR -- send failed,
    // RECV_ERROR -- recv failed,
    // CACHING -- being downloaded right now,
    // DOWNLOADING -- being downloaded right now but not intended to be kept in cache
    // CACHED -- is complete and valid,
    // FINISHED -- is complete and valid but not intended to be kept in cache

    enum CacheEntryState {
        CONNECTION_ERROR, SEND_ERROR, RECV_ERROR, CACHING, DOWNLOADING, CACHED, FINISHED
    };
private:
    std::list<Client*> _clients;
    std::map<Client*, int> _clients_bytes_got;
    Downloader *_downloader;
    CacheEntryState _state;
    Buffer *_data;
    int _bytes_received;
    BrokenUpHTTPRequest _request;
    mutable Mutex _mutex;
    mutable CondVar _cv;
public:
    CacheEntry(BrokenUpHTTPRequest request = BrokenUpHTTPRequest(""));
    ~CacheEntry();
    CacheEntryState get_state() const;
    void set_state(CacheEntryState state);
    void add_data(const char *msg);
    void add_data(const Buffer *buffer);
    const Buffer *data() const;
    void add_client(Client *client);
    void remove_client(Client *client);
    void data_got(int bytes_got, Client *client);

    void set_downloader(Downloader *downloader);
    void remove_downloader();
    bool to_delete() const;


    void lock();
    void unlock();
    void wait();
    void signal();
    void broadcast();

    int bytes_received() const;
    const BrokenUpHTTPRequest &request() const;
};
