#include "Retranslator.h"
#include "../Downloader/DownloaderRetranslator.h"
#include "../../libs/Buffer/VectorBuffer.h"
#include "../../libs/Logger/Logger.h"
#include "../Downloader/Downloader.h"
#include "../Client/Client.h"
#include "../Client/ClientRetranslator.h"

Retranslator::Retranslator(Downloader *downloader, std::list<Client*> clients, Buffer *buffer) :
_d(downloader), _c(clients), _finished(false) {
    Logger::debug("Creating retranslator");
    for (std::list<Client*>::iterator i = _c.begin();
            i != _c.end(); ++i) {
        Client *c = *i;
        c->retranslator(buffer, this);
    }
    _d->_r = this;
}

Retranslator::~Retranslator() {
    Logger::debug("Retranslator deleting");
}

void Retranslator::set_download_finished() {
    Logger::debug("Download finished");
    _finished = true;
    if(_c.size() == 0){
        delete this;
    }
}

bool Retranslator::is_download_finished() {
    return _finished;
}

void Retranslator::forward_data_to_clients(Buffer *b) {
    Logger::debug("Appending new data to clients");
    for (std::list<Client*>::iterator i = _c.begin();
            i != _c.end(); ++i) {
        Client *c = *i;
        c->_b->append(b);
    }
}

void Retranslator::remove_client(Client* c){
    Logger::debug("Client removed");
    this->_c.remove(c);
    if ((_c.size() == 0) && _finished){
        delete this;
    }
}

int Retranslator::clients_count() const {
    return _c.size();
}