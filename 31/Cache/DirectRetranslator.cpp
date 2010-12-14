#include "DirectRetranslator.h"
#include "../../libs/Logger/Logger.h"
#include <cassert>

RetranslatorState *DirectRetranslator::instance() {
    static DirectRetranslator state;
    return &state;
}

void DirectRetranslator::client_finished() {
    Logger::debug("DirectRetranslator::client_finished()");
}

void DirectRetranslator::download_add_data(const Buffer *b) {
    Logger::debug("DirectRetranslator::download_add_data()");
}

void DirectRetranslator::download_finished() {
    Logger::debug("DirectRetranslator::download_finished()");
}

void DirectRetranslator::download_connect_failed() {
    Logger::debug("DirectRetranslator::download_connect_failed()");
}

void DirectRetranslator::download_send_failed() {
    Logger::debug("DirectRetranslator::download_send_failed()");
}

void DirectRetranslator::download_recv_failed() {
    Logger::debug("DirectRetranslator::download_recv_failed()");
}