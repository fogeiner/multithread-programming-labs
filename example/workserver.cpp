#include"workserver.h"
#include"proxyexc.h"
#include"cache.h"
#include"readerrequest.h"
#include<iostream>

WorkWithServer::WorkWithServer(SelectTasks& sel, ParserHTTP& pars, unsigned short port)
    :select(sel),sock(port), parser(pars)
{
    
}

int WorkWithServer::execThis()
{
    SocketClient client;
    //std::cout << "Begin work" << std::endl;
    try
    {
	client=sock.acceptConnect();
	//std::cout << "accept" << std::endl;
    }
    catch(SockEx e)
    {
	std::cerr << "Exception in accept. Accept faild!" << std::endl;
	std::cerr << e.what() << std::endl;
	select.addRead(new WorkWithServer(*this));
	return 0;
    }
    ReaderRequest* red = new ReaderRequest(client, select, parser);
    select.addRead(red);
    select.addRead(new WorkWithServer(*this));
    //std::cout << "int WorkWithServer::execThis()" << std::endl;
    return 0;
}

int WorkWithServer::getSocketId()
{
    return sock.getId();
}

int WorkWithServer::getSocket()
{
    return sock.getId();
}

WorkWithServer::~WorkWithServer()
{

}

void WorkWithServer::cancel()
{

};
