#pragma once

#include "../libs/TCPSocket/TCPSocket.h"
#include "../libs/Buffer/VectorBuffer.h"

#include "CacheEntry.h"
#include "BrokenUpHTTPRequest.h"

struct DownloaderParameter {
    BrokenUpHTTPRequest request;
};

class Downloader {
private:
    CacheEntry *_ce;
    TCPSocket *_sock;
    Buffer *_in;
    Buffer *_out;
    void close_delete_exit();
    void remove_or_change_state(CacheEntry::CacheEntryState state);
public:
    Downloader(CacheEntry *cache_entry);
    ~Downloader();
    static Downloader *dummy_downloader();
    static void *run(void *downloader_ptr);
};
