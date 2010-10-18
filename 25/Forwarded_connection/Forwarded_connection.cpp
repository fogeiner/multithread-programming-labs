#include "Forwarded_connection.h"

void Forwarded_connection::_close_connection(int &sock, Buffer *&buf) {
	close(sock);
	sock = CLOSED_SOCKET;
	delete buf;
	buf = NULL;
}

int Forwarded_connection::client_read() {
#ifdef DEBUG
    std::clog << "client_read " << "client_socket: " << _client_socket << std::endl;
#endif
    if(_client_socket == CLOSED_SOCKET){
		return -1;
	}
    
    char b[BUFFER_SIZE];
    int read;
    read = ::recv(_client_socket, b, sizeof(b), 0);
    
    if(read == -1){
		std::cerr << "Read failure: " << strerror(errno) << std::endl;
		_close_connection(_client_socket, _server_to_client_buf);
		return -1;
	}
	
	if ((read != CONNECTION_END) && (_server_socket != CLOSED_SOCKET)){
		Chunk *chunk = new Chunk(b, read);
		_client_to_server_buf->push_back(chunk);
	} else {
		_close_connection(_client_socket, _server_to_client_buf);
	}

	return read;
}

int Forwarded_connection::server_read() {
#ifdef DEBUG
    std::clog << "server_read " << "server_socket: " << _server_socket << std::endl;
#endif
	if(_server_socket == CLOSED_SOCKET){
		return -1;
	}
	
	char b[BUFFER_SIZE];
    int read;
    read = ::recv(_server_socket, b, sizeof(b), 0);
    
    if(read == -1){
		std::cerr << "Read failure: " << strerror(errno) << std::endl;
		_close_connection(_server_socket, _client_to_server_buf);
		return -1;
	}
	
	if ((read != CONNECTION_END) && (_client_socket != CLOSED_SOCKET)){
		Chunk *chunk = new Chunk(b, read);
		_server_to_client_buf->push_back(chunk);
	} else {
		_close_connection(_server_socket, _client_to_server_buf);
	}
	
	return read;
}



int Forwarded_connection::client_write() {
#ifdef DEBUG
    std::clog << "client_write " << "client_socket: " << _client_socket << std::endl;
#endif

	if((_server_socket == CLOSED_SOCKET) && (_server_to_client_buf->size() == 0)){
		_close_connection(_client_socket, _server_to_client_buf);
	}
	
	if(_client_socket == CLOSED_SOCKET){
		return -1;
	}
	int wrote;
	
	Chunk *chunk = _server_to_client_buf->pop_front();
	const char *b = chunk->buf();
	int size = chunk->size();
	wrote = ::send(_client_socket, b, size, 0);
	
	if(wrote == -1){
		std::cerr << "Write failure: " << strerror(errno) << std::endl;
		_close_connection(_client_socket, _server_to_client_buf);
		return wrote;
	}

	_server_to_client_buf->put_back_front(chunk, wrote);

	return wrote;
}

int Forwarded_connection::server_write() {
#ifdef DEBUG
    std::clog << "server_write " << "server_socket: " << _server_socket << std::endl;
#endif

	if((_client_socket == CLOSED_SOCKET) && (_client_to_server_buf->size() == 0)){
		_close_connection(_server_socket, _client_to_server_buf);
	}

    if(_server_socket == CLOSED_SOCKET){
		return -1;
	}
    int wrote;
	
	Chunk *chunk = _client_to_server_buf->pop_front();
	const char *b = chunk->buf();
	int size = chunk->size();
	wrote = ::send(_server_socket, b, size, 0);
	
	if(wrote == -1){
		std::cerr << "Write failure " << strerror(errno) << std::endl;
		_close_connection(_server_socket, _client_to_server_buf);
		return wrote;
	}
	
	_client_to_server_buf->put_back_front(chunk, wrote);
	
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
