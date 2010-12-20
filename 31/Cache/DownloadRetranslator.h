#pragma once
#include "../../libs/Buffer/Buffer.h"

class DownloadRetranslator {
public:
    
    virtual void download_add_data(const Buffer *b) {};
    virtual void download_finished() {};
    virtual void download_connect_failed() {};
    virtual void download_send_failed() {};
    virtual void download_recv_failed() {};

    virtual ~DownloadRetranslator() {
    }
};
