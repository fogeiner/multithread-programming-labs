#include "Proxy.h"
#include "../config.h"
#include "../../libs/Logger/Logger.h"
#include "../Client/Client.h"

Proxy::Proxy() {
    this->set_reuse_addr(1);
    this->bind(ProxyConfig::listening_port);
    this->listen(ProxyConfig::backlog);
}

bool Proxy::readable() const {
    return this->is_active();
}

bool Proxy::writable() const {
    return false;
}

void Proxy::handle_accept() {
    Logger::debug("New client connected");
    new Client(this->accept());
    this->activate();
}
