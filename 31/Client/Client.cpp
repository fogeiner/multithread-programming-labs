#include "Client.h"
#include "ClientGetRequest.h"
#include "../../libs/Logger/Logger.h"
#include "../config.h"

void Client::change_state(ClientState* s) {
    this->_state = s;
}

Client::Client(TCPSocket *sock) : AsyncDispatcher(sock),
_client_retranslator(NULL),
_bytes_sent(0),
_finished(false),
_mutex(Mutex::ERRORCHECK_MUTEX) {
    _in = new VectorBuffer();
    _out = new VectorBuffer();
    this->change_state(ClientGetRequest::instance());
    this->activate();
}

Client::~Client() {
    delete _in;
    delete _out;
}

bool Client::readable() const {
    return this->_state->readable(this);
}

bool Client::writable() const {
    return this->_state->writable(this);
}

void Client::handle_read() {
    this->_state->handle_read(this);
}

void Client::handle_write() {
    this->_state->handle_write(this);
}

void Client::handle_close() {
    this->_state->handle_close(this);
}

void Client::add_data(const Buffer *b) {
    _mutex.lock();
    Logger::debug("Client::add_data(%p)", b);
    _out->append(b);
    _mutex.unlock();
}

void Client::finished() {
    Logger::debug("Client::finished()");
    _finished = true;
}


bool Client::is_finished() const {
    return _finished;
}
