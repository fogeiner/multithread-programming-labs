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

    void set_header_end_index(int index);
    Downloader *_d;
    std::list<Client*> _c;
    Buffer *_b;
    int _header_end_index;
    std::string _url;
public:

    CacheEntry(std::string url);

    ~CacheEntry();

    void retranslator();
    void activate();
    void add_client(Client *c);
    void remove_client(Client *c);
    std::string url() const;
    int header_end_index();
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