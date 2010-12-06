#include "Client.h"

#include "../Retranslator/Retranslator.h"
#include "../config.h"
#include "../../libs/Logger/Logger.h"
#include "../../libs/Buffer/VectorBuffer.h"
#include "../Cache/Cache.h"
#include "ClientError.h"
#include "ClientGettingRequest.h"
#include "ClientRetranslator.h"
#include "ClientCache.h"

void Client::change_state(ClientState* s) {
    this->_state = s;
}

Client::Client(TCPSocket *sock) : AsyncDispatcher(sock), _ce(NULL), _r(NULL), _bytes_sent(0) {
    this->_state = ClientGettingRequest::instance();
    this->_b = new VectorBuffer();
}

void Client::error(std::string msg) {
    _b->clear();
    _b->append(msg.c_str());
    this->change_state(ClientError::instance());
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

void Client::retranslator(const Buffer *b, Retranslator *r) {
    Logger::debug("Switching Client to retranslator mode");
    _b->clear();
    _b->append(b);
    _b->drop_first(this->_bytes_sent);
    _r = r;
    this->change_state(ClientRetranslator::instance());
}