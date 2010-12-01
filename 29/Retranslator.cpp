#include "Retranslator.h"
#include "../libs/Buffer/VectorBuffer.h"
#include "../libs/Logger/Logger.h"
#include "Downloader/Downloader.h"
#include "Client/Client.h"
Retranslator::Retranslator(Downloader *downloader, std::list<Client*> clients, Buffer *buffer) : _d(downloader), _c(clients), _finished(false) {
    Logger::debug("Creating retranslator");
    for (std::list<Client*>::iterator i = _c.begin();
            i != _c.end(); ++i) {
        Client *c = *i;
        c->_b->clear();
        c->_b->append(buffer);
        c->_r = this;
        c->change_state(ClientRetranslator::instance());
    }
    _d->_r = this;
    _d->change_state(DownloaderRetranslator::instance());
}

void Retranslator::download_finished() {
    _finished = true;
}

bool Retranslator::is_download_finished() {
    return _finished;
}

void Retranslator::append_data(Buffer *b) {
    Logger::debug("Appending new data to clients");
    for (std::list<Client*>::iterator i = _c.begin();
            i != _c.end(); ++i) {
        Client *c = *i;
        c->_b->append(b);
    }
}
