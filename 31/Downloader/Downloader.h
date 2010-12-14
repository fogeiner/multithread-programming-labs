#pragma once
#include "../AsyncDispatcher/AsyncDispatcher.h"
#include "../DownloadListener.h"
#include "../Cache/DownloadRetranslator.h"
#include "../BrokenUpHTTPRequest.h"

class Buffer;

class Downloader : public AsyncDispatcher, public DownloadListener {
private:
    DownloadRetranslator *_download_retranslator;
    Buffer *_in;
    Buffer *_out;
    bool _cancelled;
    void _cancel();
public:

    Downloader(BrokenUpHTTPRequest request, DownloadRetranslator *download_retranslator);
    ~Downloader();
    bool readable() const;
    bool writable() const;
    void handle_read();
    void handle_write();
    void handle_close();
    void handle_connect();

    void cancel();
};