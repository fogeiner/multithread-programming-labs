#include "CacheRetranslator.h"
#include "../../libs/Logger/Logger.h"

RetranslatorState *CacheRetranslator::instance() {
    static CacheRetranslator state;
    return &state;
}

void CacheRetranslator::add_client(Retranslator *r, ClientListener *client_listener){
    Logger::debug("CacheRetranslator::add_client()");
}

void CacheRetranslator::client_finished(Retranslator *r, ClientListener *client_listener) {
    Logger::debug("CacheRetranslator::client_finished()");
    r->delete_client(client_listener);
}

void CacheRetranslator::download_add_data(Retranslator *r, const Buffer *b) {
    Logger::debug("CacheRetranslator::download_add_data()");
}

void CacheRetranslator::download_finished(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_finished()");
}

void CacheRetranslator::download_connect_failed(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_connect_failed()");
}

void CacheRetranslator::download_send_failed(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_send_failed()");
}

void CacheRetranslator::download_recv_failed(Retranslator *r) {
    Logger::debug("CacheRetranslator::download_recv_failed()");
}