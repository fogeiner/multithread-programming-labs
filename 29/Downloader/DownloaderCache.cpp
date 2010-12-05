#include "DownloaderCache.h"
#include "Downloader.h"
#include "DownloaderState.h"
#include "../Cache/Cache.h"
#include "../../libs/Logger/Logger.h"

DownloaderCache::DownloaderCache() {
}

DownloaderState *DownloaderCache::instance() {
    static DownloaderCache dc;
    return &dc;
}

bool DownloaderCache::readable(const Downloader *d) {
    return true;
}

bool DownloaderCache::writable(const Downloader *d) {
    return false;
}

void DownloaderCache::handle_close(Downloader *d) {
    Logger::debug("DownloaderCache handle_close()");
    d->_ce->download_finished();
    d->close();
    Logger::debug("Cache download finished");
}

void DownloaderCache::handle_read(Downloader *d) {
    Logger::debug("DownloaderCache handle_read()");
    d->recv(d->_ce->get_buffer());
}
