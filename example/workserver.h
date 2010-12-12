#ifndef WORKSERVER
#define WORKSERVER

#include"cache.h"
#include"selecttask.h"
#include"socketserver.h"
#include"task.h"
#include"parser.h"

class WorkWithServer:public tasks
{
    private:
    SocketServer sock;
    SelectTasks& select;
    ParserHTTP& parser;
    
    public:
    WorkWithServer(SelectTasks& sel, ParserHTTP& pars, unsigned short port);
    int getSocketId();
    virtual int getSocket();
    virtual void cancel();
    virtual int execThis();
    ~WorkWithServer();
};

#endif
