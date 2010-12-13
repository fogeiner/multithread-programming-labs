#include "Client.h"
#include "ClientGetRequest.h"
#include "../../libs/Logger/Logger.h"
#include "../config.h"

void Client::change_state(ClientState* s) {
    this->_state = s;
}

Client::Client(TCPSocket *sock) : AsyncDispatcher(sock),
_bytes_sent(0),
_finished(false),
_cancelled(false) {
    _in = new VectorBuffer();
    _out = new VectorBuffer();
    this->change_state(ClientGetRequest::instance());
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

void Client::add_data(const Buffer *b, bool absolute) {
    Logger::debug("Client::add_data(%p, absolute=%d)", b, absolute);
    if (absolute) {
        _out->append(b);
    } else {
        Buffer *subbuf = b->last(b->size() - _bytes_sent);
        _out->append(subbuf);
        delete subbuf;
    }
}

void Client::finished() {
    Logger::debug("Client::finished()");
    _finished = true;
}

void Client::cancelled() {
    Logger::debug("Client::cancelled()");
    _cancelled = true;
}

bool Client::is_finished() const {
    return _finished;
}

bool Client::is_cancelled() const {
    return _cancelled;
}
