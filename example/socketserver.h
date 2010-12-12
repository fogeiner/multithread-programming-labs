#ifndef SOCKET_SERVER
#define SOCKET_SERVER
#include<sys/socket.h>
#include<sys/types.h>
#include"socketclient.h"

class SocketServer
{
    private:
    sockaddr_in inAddr;
    int socketId;
    static const int backlog=1;
    
    public:
    SocketServer(unsigned short port);
    SocketServer();
    int getId();
    void close();
    SocketClient acceptConnect();
    ~SocketServer();
};

#endif
