#ifndef FORWARDED_CONNECTION_H
#define FORWARDED_CONNECTION_H
#include <unistd.h>
#include <iostream>
#include "../Buffer/Buffer.h"
#include <sys/socket.h>
#include <cerrno>

class Forwarded_connection {
private:

    const static int DEFAULT_BUFFER_SIZE = 4*1024;
    const static int CONNECTION_END = 0;
    const int BUFFER_SIZE;
    int _server_socket;
    int _client_socket;

    Buffer *_client_to_server_buf;
    Buffer *_server_to_client_buf;

    void _close_connection(int &sock, Buffer *&buf);

    Forwarded_connection(const Forwarded_connection&);

    Forwarded_connection & operator=(const Forwarded_connection&);
    
public:
    const static int CLOSED_SOCKET = -1;

    Forwarded_connection(int server_sock, int client_sock, int buffer_size = DEFAULT_BUFFER_SIZE);

    ~Forwarded_connection();

    int client_socket() const;
    int server_socket() const;

    int client_read();
    int server_read();

    int client_write();
    int server_write();

    int client_to_server_msgs_count() const;
    int server_to_client_msgs_count() const;
};
#endif
