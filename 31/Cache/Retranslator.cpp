#include "Retranslator.h"
#include "RetranslatorState.h"
#include "CacheRetranslator.h"
#include "../Downloader/Downloader.h"

Retranslator::Retranslator(const BrokenUpHTTPRequest request, CacheEntry &ce, ClientListener *client_listener) :
_download_listener(DummyDownloadListener::instance()),
_response_code_received(false),
_request(request),
_ce(ce),
_clients_mutex(Mutex::RECURSIVE_MUTEX) {
    ce.caching();
    change_state(CacheRetranslator::instance());
    _clients.push_back(client_listener);
}

void Retranslator::start_download() {
    _download_listener = new Downloader(_request, this);
}

int Retranslator::clients_count() const {
    return _clients.size();
}

void Retranslator::delete_client(ClientListener *client_listener) {
    _clients_mutex.lock();
    _clients.remove(client_listener);
    _clients_mutex.unlock();
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
