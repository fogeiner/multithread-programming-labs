#include"proxyexc.h"
#include"socketserver.h"
#include<unistd.h>
#include<netdb.h>
#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>

SocketServer::SocketServer(unsigned short port)
{
    inAddr.sin_port=htons(port);
    inAddr.sin_family=AF_INET;
    inAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    socketId=socket(AF_INET, SOCK_STREAM, 0);
    if(-1==socketId)
    {
	SockEx ex;
	throw(ex);
    }
    if(-1==bind(socketId, (sockaddr*) &inAddr, sizeof(inAddr)))
    {
	close();
	SockEx ex;
	throw(ex);
    }
    if(-1==listen(socketId, backlog))
    {
	close();
	SockEx ex;
	throw(ex);
    }
};

SocketServer::SocketServer()
{

};

int SocketServer::getId()
{
    return socketId;
}

SocketClient SocketServer::acceptConnect()
{
    sockaddr_in remoteAddr;
    socklen_t lenght=sizeof(remoteAddr);
    //std::cout << "accept begin" << std::endl;
    int remSocket = accept(socketId, (sockaddr*) &remoteAddr, &lenght);
    //std::cout << "accept end" << std::endl;
    if(-1==remSocket)
    {
	SockEx ex;
	throw(ex);
    }
    return SocketClient(remSocket, &remoteAddr);
}

void SocketServer::close()
{
    ::close(socketId);
}

SocketServer::~SocketServer()
{

};
