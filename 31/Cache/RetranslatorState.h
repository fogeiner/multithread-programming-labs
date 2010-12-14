#pragma once
#include "../ClientListener.h"
class Retranslator;
class Buffer;
class RetranslatorState {
public:
    virtual void add_client(Retranslator *r, ClientListener *client_listener);
    virtual void client_finished(Retranslator *r, ClientListener *client_listener);
    virtual void download_add_data(Retranslator *r, const Buffer *b);
    virtual void download_finished(Retranslator *r);
    virtual void download_connect_failed(Retranslator *r);
    virtual void download_send_failed(Retranslator *r);
    virtual void download_recv_failed(Retranslator *r);
    virtual ~RetranslatorState(){}
};