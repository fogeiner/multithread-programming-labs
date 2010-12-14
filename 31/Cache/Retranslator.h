#pragma once

#include "ClientRetranslator.h"
#include "DownloadRetranslator.h"

class Retranslator : public ClientRetranslator, public DownloadRetranslator {
private:
    RetranslatorState *state;
    void change_state(RetranslatorState *state);
public:
    virtual void client_finished();
    virtual void download_add_data(const Buffer *b);
    virtual void download_finished();
    virtual void download_connect_failed();
    virtual void download_send_failed();
    virtual void download_recv_failed();
};