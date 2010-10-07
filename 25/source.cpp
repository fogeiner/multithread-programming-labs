#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <list>

#define DEBUG

class Chunk {
private:
    char *_buffer;
    int _size;

public:

    int size() const {
        return _size;
    }

    const char *buf() const {
        return _buffer;
    }

    Chunk(const char *buf, int size) : _size(size) {
        _buffer = new char[size];
        for (int i = 0; i < size; ++i) {
            _buffer[i] = buf[i];
        }
    }

    ~Chunk() {
        delete[] _buffer;
    }
};

class Buffer {
private:
    std::list<Chunk*> _chunks;

    Chunk *_mk_chunk(const char *buf, int size) const {
        return new Chunk(buf, size);
    }
public:

    Buffer() {

    }

    ~Buffer() {
        for (std::list<Chunk*>::iterator i = _chunks.begin();
                i != _chunks.end();) {
            Chunk *chunk = *i;
            delete chunk;
            i = _chunks.erase(i);
        }
    }

    int size() const {
        return _chunks.size();
    }

    void push_back(Chunk *chunk) {
        _chunks.push_back(chunk);
    }

    void push_front(Chunk *chunk) {
        _chunks.push_back(chunk);
    }

    void push_back(const char *buf, int size) {
        _chunks.push_back(_mk_chunk(buf, size));
    }

    void push_front(const char *buf, int size) {
        _chunks.push_front(_mk_chunk(buf, size));
    }

    Chunk* pop_back() {
        Chunk *ret_chunk_ptr = _chunks.back();
        _chunks.pop_back();
        return ret_chunk_ptr;
    }

    Chunk* pop_front() {
        Chunk *ret_chunk_ptr = _chunks.front();
        _chunks.pop_front();
        return ret_chunk_ptr;
    }
};

class Forwarded_connection {
private:

    const static int DEFAULT_BUFFER_SIZE = 1024;
    const static int CONNECTION_END = 0;
    const int BUFFER_SIZE;
    int _client_socket;
    int _server_socket;

    Buffer *_client_to_server_buf;
    Buffer *_server_to_client_buf;

    void _close_connection(int &sock, Buffer **buf) {
        close(sock);
        sock = CLOSED_SOCKET;
        delete *buf;
        *buf = NULL;
    }

    int _socket_read(int &sock, Buffer *buf_to_add, Buffer *buf_to_delete) {
        char b[BUFFER_SIZE];
        int read;
        read = ::read(sock, b, sizeof (b));

#ifdef DEBUG
        std::clog << "Read " << read << " bytes" << std::endl;
#endif
        if (read != CONNECTION_END) {
            Chunk *chunk = new Chunk(b, read);
            buf_to_add->push_back(chunk);
        } else {
            _close_connection(sock, &buf_to_delete);
        }

        return read;
    }

    int _socket_write(int &sock, int &other_sock, Buffer **buf) {
        int wrote;

        Chunk *chunk = (*buf)->pop_front();
        const char *b = chunk->buf();
        int size = chunk->size();
        wrote = ::write(sock, b, size);
#ifdef DEBUG
        std::clog << "Wrote " << wrote << " bytes" << std::endl;
#endif
        if (wrote < size) {
            Chunk *new_chunk = new Chunk((b + wrote), size - wrote);
            (*buf)->push_front(new_chunk);
        }

        delete chunk;

        if ((other_sock == Forwarded_connection::CLOSED_SOCKET) && ((*buf)->size() == 0)) {
            _close_connection(sock, buf);
        }

        return wrote;
    }

    Forwarded_connection(const Forwarded_connection&) : BUFFER_SIZE(DEFAULT_BUFFER_SIZE) {
    }

    Forwarded_connection & operator=(const Forwarded_connection&) {
    }
public:
    const static int CLOSED_SOCKET = -1;

    Forwarded_connection(int server_sock, int client_sock, int buffer_size = DEFAULT_BUFFER_SIZE) :
    BUFFER_SIZE(buffer_size),
    _server_socket(server_sock),
    _client_socket(client_sock) {
        _client_to_server_buf = new Buffer();
        _server_to_client_buf = new Buffer();
    }

    ~Forwarded_connection() {
        if (CLOSED_SOCKET != _client_socket)
            delete _client_to_server_buf;

        if (CLOSED_SOCKET != _server_socket)
            delete _server_to_client_buf;
    }

    int client_socket() const {
        return _client_socket;
    }

    int server_socket() const {
        return _server_socket;
    }

    void client_read() {
#ifdef DEBUG
        std::clog << "client_read " << "client_socket: " << _client_socket << std::endl;
#endif
        _socket_read(_client_socket, _client_to_server_buf, _server_to_client_buf);
    }

    void server_read() {
#ifdef DEBUG
        std::clog << "server_read " << "server_socket: " << _server_socket << std::endl;
#endif
        _socket_read(_server_socket, _server_to_client_buf, _client_to_server_buf);
    }

    void client_write() {
#ifdef DEBUG
        std::clog << "client_write " << "client_socket: " << _client_socket << std::endl;
#endif
        _socket_write(_client_socket, _server_socket, &_server_to_client_buf);

    }

    void server_write() {
#ifdef DEBUG
        std::clog << "server_write " << "server_socket: " << _server_socket << std::endl;
#endif
        _socket_write(_server_socket, _client_socket, &_client_to_server_buf);
    }

    int client_to_server_msgs_count() const {
        if (_client_to_server_buf != NULL) {
            return _client_to_server_buf->size();
        } else {
            return 0;
        }
    }

    int server_to_client_msgs_count() const {
        if (_server_to_client_buf != NULL) {
            return _server_to_client_buf->size();
        } else {
            return 0;
        }
    }
};

class Fd_set {
private:
    fd_set _fdset;
    int _max_fd;
public:

    Fd_set() {
        this->zero();
        _max_fd = -1;
    }

    ~Fd_set() {
    }

    fd_set& fdset() {
        return _fdset;
    }

    void zero() {
        FD_ZERO(&_fdset);
    }

    void set(int fd) {
        FD_SET(fd, &_fdset);
        if (fd > _max_fd) {
            _max_fd = fd;
        }
    }

    void clr(int fd) {
        FD_CLR(fd, &_fdset);
        // actually have to update _max_fd
    }

    int isset(int fd) {
        return FD_ISSET(fd, &_fdset);
    }

    int max_fd() const {
        return _max_fd;
    }
};

inline int max(int v1, int v2) {
    return v1 > v2 ? v1 : v2;
}

int parse_arguments(int argc, char *argv[], u_int16_t &local_port, u_int16_t &remote_port, char **remote_host) {
    const int NUM_OF_EXPECTED_ARGUMENTS = 4;
    if (argc != NUM_OF_EXPECTED_ARGUMENTS) {
        return -1;
    }

    local_port = htons(atoi(argv[1]));
    remote_port = htons(atoi(argv[3]));
    *remote_host = argv[2];
#ifdef DEBUG
    std::clog << "local port in n.rep.: " << local_port << "\n"
            << "remote port in n.rep.: " << remote_port << "\n"
            << "remote host: " << remote_host << std::endl;
#endif
}

int init_tcp_socket() {
    int listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    return listening_socket;
}

void init_local_sockaddr(sockaddr_in &local_addr, u_int16_t local_port) {
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = local_port;
    local_addr.sin_addr.s_addr = INADDR_ANY;
}

int init_remote_host_sockaddr(sockaddr_in &remote_addr, char *remote_host, u_int16_t remote_port) {
    // in static memory, no need to call free
    struct hostent *remote_hostent = gethostbyname(remote_host);

    if (remote_hostent == NULL) {
        return -1;
    }

    remote_addr.sin_addr = *((in_addr *) remote_hostent->h_addr_list[0]);
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = remote_port;
    return 0;
}

void init_fds(Fd_set &readfds, Fd_set &writefds, int listening_socket, std::list<Forwarded_connection*> &connections) {
#ifdef DEBUG
    std::clog << "Adding fds to FD_SET: " << std::endl;
#endif


    readfds.zero();
    writefds.zero();

#ifdef DEBUG
    std::clog << "read listening socket: " << listening_socket << "\n";
#endif
    readfds.set(listening_socket);

#ifdef DEBUG
    std::clog << "Forwarded connections number: " << connections.size() << std::endl;
#endif
    for (std::list<Forwarded_connection*>::iterator i = connections.begin();
            i != connections.end(); ++i) {
        Forwarded_connection *fc = *i;
#ifdef DEBUG
        std::clog << "Processing forwarded connection: " << fc
                << " server socket: " << fc->server_socket() << " client socket " << fc->client_socket()
                << std::endl;
#endif
        if ((fc->client_socket() == Forwarded_connection::CLOSED_SOCKET) &&
                (fc->server_socket() == Forwarded_connection::CLOSED_SOCKET)) {
#ifdef DEBUG
            std::clog << "deleting forwarded connection" << std::endl;
#endif
            delete fc;
            i = connections.erase(i);
            continue;
        }

        if (fc->client_socket() != Forwarded_connection::CLOSED_SOCKET) {
            readfds.set(fc->client_socket());
#ifdef DEBUG
            std::clog << "read: " << fc->client_socket() << " ";
#endif
            if (0 != fc->server_to_client_msgs_count()) {
#ifdef DEBUG
                std::clog << "write: " << fc->client_socket() << " ";
#endif
                writefds.set(fc->client_socket());
            }
        }

        if (fc->server_socket() != Forwarded_connection::CLOSED_SOCKET) {
            readfds.set(fc->server_socket());
#ifdef DEBUG
            std::clog << "read: " << fc->server_socket() << " ";
#endif

            if (0 != fc->client_to_server_msgs_count()) {
#ifdef DEBUG
                std::clog << "write: " << fc->server_socket() << " ";
#endif
                writefds.set(fc->server_socket());
            }
        }

    }

#ifdef DEBUG
    std::clog << std::endl;
#endif
}

int add_client_connection(int listening_socket, sockaddr_in &remote_addr, std::list<Forwarded_connection*> &connections) {
#ifdef DEBUG
    std::clog << "Accepting new connection" << std::endl;
#endif
    int client_sock = accept(listening_socket, NULL, NULL);
    if (-1 == client_sock) {
        return -1;
    }

    int server_sock = init_tcp_socket();
    if (-1 == server_sock) {
        return -1;
    }

    if (-1 == connect(server_sock, (const sockaddr *) & remote_addr, sizeof (remote_addr))) {
        return -1;
    }

    Forwarded_connection *fc = new Forwarded_connection(server_sock, client_sock);

    connections.push_front(fc);
#ifdef DEBUG
    std::clog << "New client connection." << "\n" << "server socket: " << server_sock
            << " client socket: " << client_sock << std::endl;
    std::clog << "Total forwarded connections: " << connections.size() << std::endl;
#endif
}

int main(int argc, char* argv[]) {
    u_int16_t remote_port;
    u_int16_t local_port;
    char *remote_host;

#ifdef DEBUG
    std::clog << "Parameters: " << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::clog << "argv[" << i << "]=" << argv[i] << std::endl;
    }
#endif

    if (-1 == parse_arguments(argc, argv, local_port, remote_port, &remote_host)) {
        std::cerr << "Usage: " << argv[0] << " local_port {remote_hostname|remote_ip} remote_port" << std::endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;

    init_local_sockaddr(local_addr, local_port);

    if (-1 == init_remote_host_sockaddr(remote_addr, remote_host, remote_port)) {
        std::cerr << "Getting remote_host info: " << hstrerror(h_errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    int listening_socket = init_tcp_socket();

    if (-1 == listening_socket) {
        std::cerr << "socket(): " << strerror(errno) << std::endl;
    }

    if (-1 == bind(listening_socket, (const sockaddr *) & local_addr, sizeof (local_addr))) {
        std::cerr << "bind(): " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    int backlog = 5;
    if (-1 == listen(listening_socket, backlog)) {
        std::cerr << "listen(): " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    Fd_set readfds, writefds;
    std::list<Forwarded_connection*> connections;

    for (;;) {
        init_fds(readfds, writefds, listening_socket, connections);

        int avaliable_sockets;

#ifdef DEBUG
        std::clog << "Getting to select" << std::endl;
#endif

        avaliable_sockets = select(max(readfds.max_fd(), writefds.max_fd()) + 1, &readfds.fdset(),
                &writefds.fdset(), NULL, NULL);

#ifdef DEBUG
        std::clog << "sockets avaliable: " << avaliable_sockets << std::endl;
#endif

        if (-1 == avaliable_sockets) {
            std::cerr << "select(): " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }

        // new client connecting
        if (readfds.isset(listening_socket)) {
            if (-1 == add_client_connection(listening_socket, remote_addr, connections)) {
                std::cerr << "initing new connection: " << strerror(errno) << std::endl;
                exit(EXIT_FAILURE);
            }
        }

#ifdef DEBUG
        std::clog << "Getting to connections forwarding" << std::endl;
#endif

        // doing avaliable work
        for (std::list<Forwarded_connection*>::iterator i = connections.begin();
                i != connections.end(); ++i) {
            Forwarded_connection *fc = *i;

            if (readfds.isset(fc->client_socket())) {
                fc->client_read();
            }

            if (readfds.isset(fc->server_socket())) {
                fc->server_read();
            }

            if (writefds.isset(fc->client_socket())) {
                fc->client_write();
            }

            if (writefds.isset(fc->server_socket())) {
                fc->server_write();
            }
        }

    }
    return (EXIT_SUCCESS);

}