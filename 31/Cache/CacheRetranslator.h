#pragma once

#include "Retranslator.h"
#include "RetranslatorState.h"

class CacheRetranslator : public RetranslatorState {
public:
    static RetranslatorState *instance();
    virtual void add_client(Retranslator *r, ClientListener *client_listener);
    virtual void client_finished(Retranslator *r, ClientListener *client_listener);
    virtual void download_add_data(Retranslator *r, const Buffer *b);
    virtual void download_finished(Retranslator *r);
    virtual void download_connect_failed(Retranslator *r);
    virtual void download_send_failed(Retranslator *r);
    virtual void download_recv_failed(Retranslator *r);
};
