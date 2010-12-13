#pragma once
#include "DownloaderState.h"

class DownloaderSendRequestRecvResponse : public DownloaderState {
public:
    static DownloaderState *instance();

    virtual bool readable(const Downloader *c);
    virtual bool writable(const Downloader *c);
    virtual void handle_read(Downloader *c);
    virtual void handle_write(Downloader *c);
    virtual void handle_close(Downloader *c);
    virtual void handle_connect(Downloader *c);
};
