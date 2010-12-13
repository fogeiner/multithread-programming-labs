#pragma once
#include "../AsyncDispatcher/AsyncDispatcher.h"
#include "DownloaderState.h"

class Downloader : public AsyncDispatcher {
    friend class DownloaderState;

    void change_state(DownloaderState* s);
    DownloaderState *_state;

public:

    Downloader();
    ~Downloader();
    bool readable() const;
    bool writable() const;
    void handle_read();
    void handle_write();
    void handle_close();
    void handle_connect();
};