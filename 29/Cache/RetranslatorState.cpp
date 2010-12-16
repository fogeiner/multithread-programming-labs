#include "RetranslatorState.h"
#include "../../libs/Logger/Logger.h"

void RetranslatorState::add_client(Retranslator *r, ClientListener *client_listener){
    Logger::info("RetranslatorState unhandled add_client()");
}

void RetranslatorState::client_finished(Retranslator *r, ClientListener *client_listener) {
    Logger::info("RetranslatorState unhandled client_finished()");
}

void RetranslatorState::download_add_data(Retranslator *r, const Buffer *b) {
    Logger::info("RetranslatorState unhandled download_add_data()");
}

void RetranslatorState::download_finished(Retranslator *r) {
    Logger::info("RetranslatorState unhandled download_finished()");
}

void RetranslatorState::download_connect_failed(Retranslator *r) {
    Logger::info("RetranslatorState unhandled download_connect_failed()");
}

void RetranslatorState::download_send_failed(Retranslator *r) {
    Logger::info("RetranslatorState unhandled download_send_failed()");
}

void RetranslatorState::download_recv_failed(Retranslator *r) {
    Logger::info("RetranslatorState unhandled download_recv_failed()");
}
