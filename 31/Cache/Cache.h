#pragma once
#include <cassert>
#include <list>
#include <map>
#include <string>
class Downloader;
class Client;
class Buffer;

class CacheEntry {
    friend class Client;
    friend class Downloader;
private:

    Downloader *_d;
    std::list<Client*> _c;

    Buffer *_query;
    Buffer *_b;

    std::string _url;
public:

    CacheEntry(std::string url, const Buffer *b);

    ~CacheEntry();
    const Buffer *get_query();
    Buffer *get_buffer();
    int data_size() const;
    void download_finished();
    bool is_download_finished() const;
    void start_retranslator();
    void activate();
    void add_client(Client *c);
    void remove_client(Client *c);
    std::string url() const;
};

class Cache {
private:

    Cache();
    static std::map<std::string, CacheEntry*> _c;
public:

    static Cache *instance();
    static CacheEntry *get(std::string key) ;
    static void add(std::string key, CacheEntry *ce);
    static void remove(std::string key);
};