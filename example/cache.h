#ifndef CACHE
#define CACHE
#include<map>
#include<deque>
#include<vector>
#include"data.h"
#include"socketclient.h"
#include"selecttask.h"
#include"task.h"
#include"getfromcache.h"
#include"parseresult.h"
#include"listeners.h"
#include"pageid.h"
#include<string>

class Cache
{
    private:
    std::map<PageId, Data> memcache;
    std::map<PageId, Listeners> listeners;
    std::deque<PageId> pages;
    SelectTasks& select;
    pthread_mutex_t cacheLock;
    
    int cacheSize;
    double downloadInf;
    void getExistElement(SocketClient& sock, std::string request, const PageId& page);
    void getNewPage(SocketClient& sock, std::string request, std::string host, const PageId& page);
    
    public:
    static const int MAX_CACHE_SIZE = 1*1024*1024*1024;
    static const std::string NOT_IMPLEMENTED;
    static const std::string NOT_SUPPORTED;
    static const std::string NOT_FOUND;
    
    Cache(SelectTasks& select);
    void setCached(PageId& page, bool value);
    void get(SocketClient& sock, ParseResult result);
    void add(PageId page, void* data, int length, bool end);    
    void remove(PageId page, tasks* task);
    void cancel(PageId& page);
    void check(PageId& page);
    void deleteDownloaders(PageId& page);
    
    double getCacheSize() const;
    int getCountRecords();
    double getDownloadInf() const;
    
    ~Cache();
};

#endif
