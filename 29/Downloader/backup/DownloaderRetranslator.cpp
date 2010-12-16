#include "DownloaderRetranslator.h"
#include "Downloader.h"
#include "../Retranslator/Retranslator.h"
#include "../../libs/Logger/Logger.h"

DownloaderRetranslator::DownloaderRetranslator() {
}

DownloaderState *DownloaderRetranslator::instance() {
    static DownloaderRetranslator dr;
    return &dr;
}

bool DownloaderRetranslator::readable(const Downloader *d) {
    return d->is_active();
}

bool DownloaderRetranslator::writable(const Downloader *d) {
    return false;
}

void DownloaderRetranslator::handle_close(Downloader *d) {
    Logger::debug("DownloaderRetranslator::handle_close(); finished");
    
    d->_r->set_download_finished();
    d->close();
}

void DownloaderRetranslator::handle_read(Downloader *d) {
    if(d->_r->clients_count() == 0){
        Logger::debug("DownloaderRetranslator::handle_read(); stopping due to no clients");
        d->close();
        return;
    }
    d->_out->clear();
    Logger::debug("DownloaderRetranslator::handle_read(); receiving data");
    d->recv(d->_out);
    d->_r->forward_data_to_clients(d->_out);
}

