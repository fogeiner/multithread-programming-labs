#include "Retranslator.h"
#include "RetranslatorState.h"

void Retranslator::delete_client(ClientListener *client_listener) {
    _clients.remove(client_listener);
}

void Retranslator::add_client(ClientListener *client_listener) {
    _clients.push_back(client_listener);
}

void Retranslator::change_state(RetranslatorState *state) {
    this->state = state;
}

void Retranslator::client_finished() {
    this->state->client_finished(this);
}

void Retranslator::download_add_data(const Buffer *b) {
    this->state->download_add_data(this, b);
}

void Retranslator::download_finished() {
    this->state->download_finished(this);
}

void Retranslator::download_connect_failed() {
    this->state->download_connect_failed(this);
}

void Retranslator::download_send_failed() {
    this->state->download_send_failed(this);
}

void Retranslator::download_recv_failed() {
    this->state->download_recv_failed(this);
}
