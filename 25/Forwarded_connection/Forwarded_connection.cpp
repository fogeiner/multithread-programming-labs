#include "Forwarded_connection.h"

void Forwarded_connection::_close_connection(int &sock, Buffer **buf) {
    if(sock != CLOSED_SOCKET){
		close(sock);
		sock = CLOSED_SOCKET;
		delete *buf;
		*buf = NULL;
	}
}

int Forwarded_connection::_socket_read(int &sock, Buffer *buf_to_add, Buffer *buf_to_delete) {

    char b[BUFFER_SIZE];
    int read;
    read = ::read(sock, b, sizeof (b));

	if(read == -1){
		std::cerr << "Read failure" << std::endl;
		_close_connection(sock, &buf_to_delete);
		return -1;
	}

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

int Forwarded_connection::_socket_write(int &sock, int &other_sock, Buffer **buf) {
	if(sock == CLOSED_SOCKET){
		return -1;
	}
	// clients socket was closed due to an error
	if(*buf == NULL){
		_close_connection(sock, buf);
		return -1;
	}

    int wrote;

    Chunk *chunk = (*buf)->pop_front();
    const char *b = chunk->buf();
    int size = chunk->size();
    wrote = ::write(sock, b, size);
    
    if(wrote == -1){
		std::cerr << "Write failure" << std::endl;
		_close_connection(sock, buf);
		return -1;
	}
    
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

Forwarded_connection::Forwarded_connection(int server_sock, int client_sock, int buffer_size) :
BUFFER_SIZE(buffer_size),
_server_socket(server_sock),
_client_socket(client_sock) {
    _client_to_server_buf = new Buffer();
    _server_to_client_buf = new Buffer();
}

Forwarded_connection::~Forwarded_connection() {
    if (CLOSED_SOCKET != _client_socket)
        delete _client_to_server_buf;

    if (CLOSED_SOCKET != _server_socket)
        delete _server_to_client_buf;
}

int Forwarded_connection::client_socket() const {
    return _client_socket;
}

int Forwarded_connection::server_socket() const {
    return _server_socket;
}

void Forwarded_connection::client_read() {
#ifdef DEBUG
    std::clog << "client_read " << "client_socket: " << _client_socket << std::endl;
#endif
    _socket_read(_client_socket, _client_to_server_buf, _server_to_client_buf);
}

void Forwarded_connection::server_read() {
#ifdef DEBUG
    std::clog << "server_read " << "server_socket: " << _server_socket << std::endl;
#endif
    _socket_read(_server_socket, _server_to_client_buf, _client_to_server_buf);
}

void Forwarded_connection::client_write() {
#ifdef DEBUG
    std::clog << "client_write " << "client_socket: " << _client_socket << std::endl;
#endif
    _socket_write(_client_socket, _server_socket, &_server_to_client_buf);

}

void Forwarded_connection::server_write() {
#ifdef DEBUG
    std::clog << "server_write " << "server_socket: " << _server_socket << std::endl;
#endif
    _socket_write(_server_socket, _client_socket, &_client_to_server_buf);
}

int Forwarded_connection::client_to_server_msgs_count() const {
    if (_client_to_server_buf != NULL) {
        return _client_to_server_buf->size();
    } else {
        return 0;
    }
}

int Forwarded_connection::server_to_client_msgs_count() const {
    if (_server_to_client_buf != NULL) {
        return _server_to_client_buf->size();
    } else {
        return 0;
    }
}
