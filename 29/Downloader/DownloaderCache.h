#pragma once
#include "DownloaderState.h"

class Downloader;
class DownloaderCache : public DownloaderState {
private:

    DownloaderCache();
public:

    DownloaderState *instance();

    bool readable(const Downloader *d);

    bool writable(const Downloader *d);

    void handle_close(Downloader *d);
    void handle_read(Downloader *d);
};