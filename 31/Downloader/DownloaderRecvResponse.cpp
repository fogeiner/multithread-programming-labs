#include "DownloaderRecvResponse.h"

DownloaderState *instance() {
    DownloaderRecvResponse state;
    return &state;
}

bool DownloaderRecvResponse::readable(const Downloader *c) {

}

bool DownloaderRecvResponse::writable(const Downloader *c) {

}

void DownloaderRecvResponse::handle_read(Downloader *c) {
    Logger::debug("DownloaderRecvResponse::handle_read()");
}

void DownloaderRecvResponse::handle_write(Downloader *c) {
    Logger::debug("DownloaderRecvResponse::handle_write()");
}

void DownloaderRecvResponse::handle_close(Downloader *c) {
    Logger::debug("DownloaderRecvResponse::handle_close()");
}
