#ifndef FORWARDED_CONNECTION_H
#define FORWARDED_CONNECTION_H
#include <unistd.h>
#include "../../libs/Buffer/Buffer.h"

class Forwarded_connection {
private:

    const static int DEFAULT_BUFFER_SIZE = 1024;
    const static int CONNECTION_END = 0;
    const int BUFFER_SIZE;
    int _client_socket;
    int _server_socket;

    Buffer *_client_to_server_buf;
    Buffer *_server_to_client_buf;

    void _close_connection(int &sock, Buffer **buf);

    int _socket_read(int &sock, Buffer *buf_to_add, Buffer *buf_to_delete);

    int _socket_write(int &sock, int &other_sock, Buffer **buf);

    Forwarded_connection(const Forwarded_connection&);

    Forwarded_connection & operator=(const Forwarded_connection&);
public:
    const static int CLOSED_SOCKET = -1;

    Forwarded_connection(int server_sock, int client_sock, int buffer_size = DEFAULT_BUFFER_SIZE);

    ~Forwarded_connection();

    int client_socket() const;
    int server_socket() const;
    void client_read();

    void server_read();

    void client_write();
    void server_write();

    int client_to_server_msgs_count() const;

    int server_to_client_msgs_count() const;

};
#endif