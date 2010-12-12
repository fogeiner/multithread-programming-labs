#ifndef GETFROMCACHE
#define GETFROMCACHE
#include"task.h"
#include"selecttask.h"
#include"pageid.h"
#include"socketclient.h"

class GetFromCache : public tasks
{
    private:
    SocketClient sock;
    SelectTasks& select;
    pthread_mutex_t forData;
    void* data;
    int length;
    bool ended;
    bool inQueue;
    //PageId page;
    bool isCancelled;
    
    public:
    GetFromCache(SocketClient socketId, SelectTasks& sel/*, PageId pag*/);
    void addData(void* data, int length, bool end);
    virtual int getSocket();
    virtual void cancel();
    virtual int execThis();
    
    bool canceled();
};

#endif
