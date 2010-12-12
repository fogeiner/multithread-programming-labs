#ifndef SOCKETCLIENT
#define SOCKETCLIENT
#include<sys/socket.h>
#include<netinet/in.h>

class SocketClient
{
    private:
    sockaddr_in inAddr;
    int socketId;
    
    public:
    static int countOpenSocket;
    
    SocketClient(unsigned short port, const char* addr);
    SocketClient(int socket, const sockaddr_in* addr);
    SocketClient();
    
    int getId() const;
    void readForSoc(void* data, int& length);
    void writeForSoc(const void* data, int& length);
    void close();
    ~SocketClient();
};

#endif
