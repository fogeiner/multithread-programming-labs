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
    return true;
}

bool DownloaderRetranslator::writable(const Downloader *d) {
    return false;
}

void DownloaderRetranslator::handle_close(Downloader *d) {
    d->_r->download_finished();
    d->close();
}

void DownloaderRetranslator::handle_read(Downloader *d) {
    d->_out->clear();
    d->recv(d->_out);
    d->_r->append_data(d->_out);
}

