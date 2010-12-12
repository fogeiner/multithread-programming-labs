#ifndef DOWNLOADER
#define DOWNLOADER

#include"task.h"
#include"socketclient.h"
#include"pageid.h"
#include"cache.h"
#include"selecttask.h"

class DownloadRequest: public tasks
{
    private:
    SocketClient sock;
    std::string request;
    int write;
    SelectTasks& select;
    bool canceled;
    
    public:
    DownloadRequest(SocketClient socket, std::string req, SelectTasks& sel);
    virtual int getSocket();
    virtual void cancel();
    virtual int execThis();
};

class DownloadAnswer: public tasks
{
    private:
    SocketClient sock;
    SelectTasks& select;
    PageId page;
    Cache* cache;
    bool first;
    bool canceled;
    
    public:
    DownloadAnswer(SocketClient socket, SelectTasks& sel, PageId pages, Cache* cach);
    virtual int getSocket();
    virtual void cancel();
    virtual int execThis();
    ~DownloadAnswer();
};

#endif
