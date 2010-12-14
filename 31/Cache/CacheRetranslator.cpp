#include "CacheRetranslator.h"
#include "../../libs/Logger/Logger.h"

RetranslatorState *CacheRetranslator::instance() {
    static CacheRetranslator state;
    return &state;
}

void CacheRetranslator::client_finished() {
    Logger::debug("CacheRetranslator::client_finished()");
}

void CacheRetranslator::download_add_data(const Buffer *b) {
    Logger::debug("CacheRetranslator::download_add_data()");
}

void CacheRetranslator::download_finished() {
    Logger::debug("CacheRetranslator::download_finished()");
}

void CacheRetranslator::download_connect_failed() {
    Logger::debug("CacheRetranslator::download_connect_failed()");
}

void CacheRetranslator::download_send_failed() {
    Logger::debug("CacheRetranslator::download_send_failed()");
}

void CacheRetranslator::download_recv_failed() {
    Logger::debug("CacheRetranslator::download_recv_failed()");
}