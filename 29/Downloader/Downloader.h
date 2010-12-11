#pragma once
#include "../../libs/AsyncDispatcher/AsyncDispatcher.h"
#include "DownloaderState.h"

class Downloader;
class Retranslator;
class CacheEntry;

class Downloader : public AsyncDispatcher {
    friend class DownloaderState;
    friend class DownloaderCache;
    friend class DownloaderRequestResponse;
    friend class DownloaderRetranslator;
    friend class Retranslator;

    void change_state(DownloaderState* s);

    DownloaderState *_state;

    Buffer *_in;
    Buffer *_out;

    Retranslator *_r;
    CacheEntry *_ce;

public:

    Downloader(CacheEntry *ce);
    ~Downloader();

    bool readable() const;
    bool writable() const;
    void handle_read();
    void handle_write();
    void handle_close();
    void handle_connect();
};

