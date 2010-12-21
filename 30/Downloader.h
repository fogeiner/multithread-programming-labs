#pragma once

#include "../libs/TCPSocket/TCPSocket.h"
#include "../libs/Buffer/VectorBuffer.h"
#include "BrokenUpHTTPRequest.h"

class CacheEntry;

struct DownloaderParameter {
    BrokenUpHTTPRequest request;
};

class Downloader {
private:
    CacheEntry *_ce;
    TCPSocket *_sock;
    Buffer *_in;
    Buffer *_out;
public:
    Downloader(CacheEntry *cache_entry);
    ~Downloader();
    static void *run(void *downloader_ptr);


};
