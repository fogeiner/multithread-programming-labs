#ifndef READERREQUEST
#define READERREQUEST

#include"task.h"
#include"selecttask.h"
#include"socketclient.h"
#include"parser.h"
#include<string>

class ReaderRequest : public tasks
{
    private:
    SocketClient sock;
    SelectTasks& select;
    std::string mes;
    ParserHTTP& parse;
    
    public:
    ReaderRequest(SocketClient soc, SelectTasks& sel, ParserHTTP& pars);
    ParseResult getRequest();
    virtual int getSocket();
    virtual void cancel();
    virtual int execThis();
};

#endif
