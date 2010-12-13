#pragma once
#include "DownloaderState.h"
class Downloader;

class DownloaderRetranslator : public DownloaderState {
private:

    DownloaderRetranslator();
public:

    static DownloaderState *instance();
    bool readable(const Downloader *d);
    bool writable(const Downloader *d);
    void handle_close(Downloader *d);
    void handle_read(Downloader *d);
};
