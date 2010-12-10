#include "../../libs/Logger/Logger.h"

#include "Downloader.h"
#include "DownloaderState.h"

bool DownloaderState::readable(const Downloader *c) {
    Logger::info("DownloaderState unhandled readable()");
    return false;
}

bool DownloaderState::writable(const Downloader *c) {
    Logger::info("DownloaderState unhandled writable()");
    return false;
}

void DownloaderState::handle_read(Downloader *c) {
    Logger::info("DownloaderState unhandled handle_read()");
}

void DownloaderState::handle_write(Downloader *c) {
    Logger::info("DownloaderState unhandled handle_write()");
}

void DownloaderState::handle_close(Downloader *c) {
    Logger::info("DownloaderState unhandled handle_close()");
}

void DownloaderState::handle_connect(Downloader *c) {
    Logger::info("DownloaderState unhandled handle_connect()");
}

void DownloaderState::change_state(Downloader *d, DownloaderState *s) {
    d->change_state(s);
}
