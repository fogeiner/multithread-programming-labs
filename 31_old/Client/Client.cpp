#include "Client.h"

#include "../../libs/Logger/Logger.h"
#include "../../libs/Buffer/VectorBuffer.h"

#include "../Cache/Cache.h"
#include "../Retranslator/Retranslator.h"

#include "ClientError.h"
#include "ClientGettingRequest.h"
#include "ClientRetranslator.h"
#include "ClientCache.h"

#include "../config.h"

void Client::change_state(ClientState* s) {
    this->_state = s;
}

Client::Client(TCPSocket *sock) : AsyncDispatcher(sock), _ce(NULL), _r(NULL), _bytes_sent(0) {
    this->_state = ClientGettingRequest::instance();
    this->_b = new VectorBuffer();
}

Client::~Client(){
    delete this->_b;
}

void Client::error(std::string msg) {
    _b->clear();
    _b->append(msg.c_str());
    this->change_state(ClientError::instance());
}

bool Client::readable() const {
    return this->is_active() && this->_state->readable(this);
}

bool Client::writable() const {
    return this->is_active() && this->_state->writable(this);
}

void Client::handle_read() {
    this->_state->handle_read(this);
    this->activate();
}

void Client::handle_write() {
    this->_state->handle_write(this);
    this->activate();
}

void Client::handle_close() {
    this->_state->handle_close(this);
}

void Client::switch_to_retranslator(const Buffer *b, Retranslator *r) {
    Logger::debug("Switching Client to retranslator mode");
    _b->clear();
    _b->append(b);
    _b->drop_first(this->_bytes_sent);
    _r = r;
    this->change_state(ClientRetranslator::instance());
}