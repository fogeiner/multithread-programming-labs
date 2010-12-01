#include "DownloaderRequestResponse.h"
#include "Downloader.h"
#include "../Cache/Cache.h"
#include "DownloaderState.h"
#include "../../libs/Logger/Logger.h"

DownloaderRequestResponse::DownloaderRequestResponse() {
}

DownloaderState *DownloaderRequestResponse::instance() {
    static DownloaderRequestResponse drr;
    return &drr;
}

bool DownloaderRequestResponse::readable(const Downloader *d) {
    return d->_out->size() == 0;
}

bool DownloaderRequestResponse::writable(const Downloader *d) {
    return d->_out->size() > 0;
}

void DownloaderRequestResponse::handle_close(Downloader *d) {

    Logger::debug("DownloaderRequestResponse handle_close()");
    // XXX set the status of CacheEntry
    d->close();
}

void DownloaderRequestResponse::handle_read(Downloader *d) {

    Logger::debug("DownloaderRequestResponse handle_read()");
    d->recv(d->_in);

    const char *p;
    // XXX
    if ((p = strstr(d->_in->buf(), "\r\n\r\n")) != NULL) {
        Logger::debug("Downloader found end of a response header");
        d->set_header_end_index(p - d->_in->buf() + sizeof ("\r\n\r\n"));

        // HTTP/1.x 200
        if (strstr(d->_in->buf(), "200") != d->_in->buf() + sizeof ("HTTP/1.x")) {
            Logger::debug("Response is not 200");
            // switching to retranslator mode
            d->_ce->retranslator();
        } else {
            Logger::debug("Response is 200");
            assert(false);
            // switching to cache mode
        }
    }
}

void DownloaderRequestResponse::handle_write(Downloader *d) {
    Logger::debug("DownloaderRequestResponse handle_write()");
    int sent;
    sent = d->send(d->_out);
    d->_out->drop_first(sent);
}

