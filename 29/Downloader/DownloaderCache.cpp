#include "DownloaderCache.h"
#include "Downloader.h"
#include "DownloaderState.h"

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

}

void DownloaderCache::handle_read(Downloader *d) {

}
