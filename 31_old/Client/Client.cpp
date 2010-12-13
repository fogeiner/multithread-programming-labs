#include "Client.h"
#include "ClientGetRequest.h"
#include "../../libs/Logger/Logger.h"
#include "../config.h"

void Client::change_state(ClientState* s) {
    this->_state = s;
}

Client::Client(TCPSocket *sock) : AsyncDispatcher(sock) {
    this->change_state(ClientGetRequest::instance());
}

Client::~Client() {
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
