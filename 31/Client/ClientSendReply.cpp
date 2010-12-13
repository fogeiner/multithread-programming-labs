#include "ClientSendReply.h"

bool ClientSendReply::readable(const Client *c) {
    return false;
}

bool ClientSendReply::writable(const Client *c) {
    // depends on _out Buffer
    return true;
}

void ClientSendReply::handle_write(Client *c) {
    Logger::debug("ClientSendReply::handle_write");
}

void ClientSendReply::handle_close(Client *c) {
    Logger::debug("ClientSendReply::handle_close");
}
