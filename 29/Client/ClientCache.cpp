#include "ClientCache.h"
#include "Client.h"

ClientCache::ClientCache() {
}

ClientState *ClientCache::instance() {
    static ClientCache cc;
    return &cc;
}

bool ClientCache::readable(const Client *c) {
    return false;
}

bool ClientCache::writable(const Client *c) {
    return true; // XXX true!

}

void ClientCache::handle_write(Client *c) {
}
