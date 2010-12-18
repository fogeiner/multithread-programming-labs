#include "Retranslator.h"
#include "RetranslatorState.h"
#include "CacheRetranslator.h"
#include "../Downloader/Downloader.h"

Retranslator::Retranslator(const BrokenUpHTTPRequest request, CacheEntry &ce, ClientListener *client_listener) :
_download_listener(NULL),
_response_code_received(false),
_request(request),
_ce(ce),
_mutex(Mutex::RECURSIVE_MUTEX) {
    change_state(CacheRetranslator::instance());
    _clients.push_back(client_listener);
    _download_listener = new Downloader(request, this);
    ce.caching();
}

int Retranslator::clients_count() const {
    return _clients.size();
}

void Retranslator::delete_client(ClientListener *client_listener) {
    _mutex.lock();
    _clients.remove(client_listener);
    _mutex.unlock();
}

void Retranslator::add_client(ClientListener *client_listener) {
    this->_state->add_client(this, client_listener);
}

void Retranslator::change_state(RetranslatorState *state) {
    this->_state = state;
}

void Retranslator::client_finished(ClientListener *client_listener) {
    this->_state->client_finished(this, client_listener);
}

void Retranslator::download_add_data(const Buffer *b) {
    this->_state->download_add_data(this, b);
}

void Retranslator::download_finished() {
    this->_state->download_finished(this);
}

void Retranslator::download_connect_failed() {
    this->_state->download_connect_failed(this);
}

void Retranslator::download_send_failed() {
    this->_state->download_send_failed(this);
}

void Retranslator::download_recv_failed() {
    this->_state->download_recv_failed(this);
}
