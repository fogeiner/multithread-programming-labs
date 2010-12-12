#include "DownloaderCache.h"
#include "Downloader.h"
#include "DownloaderState.h"
#include "DownloaderRetranslator.h"
#include "../config.h"
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
    Logger::debug("DownloaderCache::handle_close()");
    d->_ce->download_finished();
    d->close();
    Logger::debug("Cache download finished");
}

void DownloaderCache::handle_read(Downloader *d) {
    try {
        Logger::debug("DownloaderCache::handle_read()");
        d->recv(d->_ce->get_buffer());

        if (d->_ce->data_size() > ProxyConfig::max_cache_entry_size) {
            Logger::debug("Max CacheEntry size exceeded");
            Cache::instance()->remove(d->_ce->url());

            // switching to retranslator mode
            // and removing entry from cache
            d->_ce->start_retranslator();
            delete d->_ce;

            Logger::debug("Changing Downloader state to retranslator mode");
            d->change_state(DownloaderRetranslator::instance());
        }
    } catch (RecvException &ex) {
        Logger::error(ex.what());
        d->_ce->download_finished();
        d->close();
    }
}
