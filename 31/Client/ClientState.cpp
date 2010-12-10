#include "ClientState.h"
#include "Client.h"
#include "../../libs/Logger/Logger.h"

bool ClientState::readable(const Client *c) {
    Logger::info("ClientState unhandled readable()");
    return false;
}

bool ClientState::writable(const Client *c) {
    Logger::info("ClientState unhandled writable()");
    return false;
}

void ClientState::handle_read(Client *c) {
    Logger::info("ClientState unhandled handle_read()");
}

void ClientState::handle_write(Client *c) {
    Logger::info("ClientState unhandled handle_write()");
}

void ClientState::handle_close(Client *c) {
    Logger::info("ClientState unhandled handle_close()");
}

void ClientState::handle_connect(Client *c) {
    Logger::info("ClientState unhandled handle_connect()");
}

void ClientState::change_state(Client *c, ClientState *s) {
    c->change_state(s);
}
