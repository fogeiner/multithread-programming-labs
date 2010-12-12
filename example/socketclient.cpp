#define _REENTRANT true
#include<cstring>
#include"socketclient.h"
#include<netdb.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include"proxyexc.h"
#include<errno.h>
#include<signal.h>
#include<unistd.h>
#include<iostream>
#include<strings.h>

int SocketClient::countOpenSocket = 0;

SocketClient::SocketClient(unsigned short port, const char* addr)
{
    inAddr.sin_port = htons(port);
    inAddr.sin_family = AF_INET;
    hostent host, *phost, *result;
    int code;
    const int SIZE_B=1024;
    char buf[SIZE_B];
    if(-1 == gethostbyname_r(addr, &host, buf, SIZE_B, &result, &code))
    {
	//std::cout << "a " << addr << std::endl;
	throw(SockEx(hstrerror(code)));
    }
    phost=result;
    memcpy(&inAddr.sin_addr, phost->h_addr, sizeof(phost->h_addr));
    socketId=socket(AF_INET, SOCK_STREAM, 0);
    if(-1==socketId)
    {
	//std::cout << "b" << std::endl;
	SockEx e;
	throw(e);
    }
    fcntl(socketId, F_SETFL, O_NONBLOCK);
    if(-1==connect(socketId, (sockaddr*)&inAddr, sizeof(inAddr)))
    {
	countOpenSocket++;
	if(EINPROGRESS==errno)
	{
	    return;
	}
	close();
	//std::cout << "c" << std::endl;
	SockEx e;
	throw(e);
    }
    countOpenSocket++;
}

SocketClient::SocketClient()
{

}

SocketClient::SocketClient(int socket, const sockaddr_in* remAddr)
{
    countOpenSocket++;
    socketId=socket;
    memcpy(&(inAddr), remAddr, sizeof(inAddr));
}

int SocketClient::getId() const
{
    return socketId;
}

void SocketClient::readForSoc(void* data, int& length)
{
    if(0>(length=recv(socketId, data, length, 0)))
    {
	SockEx e;
	throw(e);
    } 
    return;
}

void SocketClient::writeForSoc(const void* data, int& length)
{
    int code;
    socklen_t len=sizeof(code);
    if((-1==getsockopt(socketId, SOL_SOCKET, SO_ERROR, (void*)&code, &len)) || (0!=code))
    {
	SockEx e;
	throw(e);
	return;
    }
    signal(SIGPIPE, SIG_IGN);
    if(-1==(length=send(socketId, data, length, 0)))
    {
	SockEx e;
	throw e;
	return;
    }
}

void SocketClient::close()
{
    countOpenSocket--;
    ::close(socketId);
}
        
SocketClient::~SocketClient()
{

}
