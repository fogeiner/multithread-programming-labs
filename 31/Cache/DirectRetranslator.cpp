#include "DirectRetranslator.h"
#include "../../libs/Logger/Logger.h"
#include <cassert>

RetranslatorState *DirectRetranslator::instance() {
    static DirectRetranslator state;
    return &state;
}

void DirectRetranslator::add_client(Retranslator *r, ClientListener *client_listener){
    Logger::debug("DirectRetranslator::add_client()");
}

void DirectRetranslator::client_finished(Retranslator *r, ClientListener *client_listener) {
    Logger::debug("DirectRetranslator::client_finished()");
}

void DirectRetranslator::download_add_data(Retranslator *r, const Buffer *b) {
    Logger::debug("DirectRetranslator::download_add_data()");
}

void DirectRetranslator::download_finished(Retranslator *r) {
    Logger::debug("DirectRetranslator::download_finished()");
}

void DirectRetranslator::download_connect_failed(Retranslator *r) {
    Logger::debug("DirectRetranslator::download_connect_failed()");
}

void DirectRetranslator::download_send_failed(Retranslator *r) {
    Logger::debug("DirectRetranslator::download_send_failed()");
}

void DirectRetranslator::download_recv_failed(Retranslator *r) {
    Logger::debug("DirectRetranslator::download_recv_failed()");
}