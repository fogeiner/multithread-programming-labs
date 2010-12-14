#pragma once
#include "../../libs/Buffer/Buffer.h"

class DownloadRetranslator {
public:
    
    virtual void download_add_data(const Buffer *b) = 0;
    virtual void download_finished() = 0;
    virtual void download_connect_failed() = 0;
    virtual void download_send_failed() = 0;
    virtual void download_recv_failed() = 0;

    virtual ~DownloadRetranslator() {
    }
};