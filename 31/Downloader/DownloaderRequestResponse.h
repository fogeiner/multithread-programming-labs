#pragma once
#include "DownloaderState.h"

class Downloader;

class DownloaderRequestResponse : public DownloaderState {
private:

    DownloaderRequestResponse();
public:

    static DownloaderState *instance();

    bool readable(const Downloader *d);
    bool writable(const Downloader *d);
    void handle_close(Downloader *d);
    void handle_read(Downloader *d);
    void handle_write(Downloader *d);
};
