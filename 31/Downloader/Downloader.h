#pragma once
#include "../AsyncDispatcher/AsyncDispatcher.h"
#include "DownloaderState.h"
#include "../../libs/Buffer/VectorBuffer.h"

class Downloader : public AsyncDispatcher {
    friend class DownloaderState;
    friend class DownloaderSendRequestRecvResponse;
    friend class DownloaderRecvResponse;

    void change_state(DownloaderState* s);
    DownloaderState *_state;

    Buffer *_in;
    Buffer *_out;
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