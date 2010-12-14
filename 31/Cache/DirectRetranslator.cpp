#include "DirectRetranslator.h"
#include <cassert>

// delete client from list; if no clients left,
// cancel download
void DirectRetranslator::client_finished() {
}

// supply clients with data
void DirectRetranslator::download_add_data(const Buffer *b) {
}

// clients finished; delete
void DirectRetranslator::download_finished() {
}

void DirectRetranslator::download_connect_failed() {
    assert(false);
}

void DirectRetranslator::download_send_failed() {
    assert(false);
}
// clients finished; delete
void DirectRetranslator::download_recv_failed() {
}