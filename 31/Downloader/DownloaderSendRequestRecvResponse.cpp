#include "DownloaderSendRequestRecvResponse.h"
#include "../../libs/Logger/Logger.h"

DownloaderState *DownloaderSendRequestRecvResponse::instance() {
    static DownloaderSendRequestRecvResponse state;
    return &state;
}

bool DownloaderSendRequestRecvResponse::readable(const Downloader *c) {

}

bool DownloaderSendRequestRecvResponse::writable(const Downloader *c) {

}

void DownloaderSendRequestRecvResponse::handle_read(Downloader *c) {
    Logger::debug("DownloaderSendRequestRecvResponse::handle_read()");
}

void DownloaderSendRequestRecvResponse::handle_write(Downloader *c) {
    Logger::debug("DownloaderSendRequestRecvResponse::handle_write()");

}

void DownloaderSendRequestRecvResponse::handle_close(Downloader *c) {
    Logger::debug("DownloaderSendRequestRecvResponse::handle_close()");
}

void DownloaderSendRequestRecvResponse::handle_connect(Downloader *c) {
    Logger::debug("DownloaderSendRequestRecvResponse::handle_connect()");

}