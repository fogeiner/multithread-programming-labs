#pragma once
#include "../ClientListener.h"

#include "ClientRetranslator.h"
#include "DownloadRetranslator.h"
#include "RetranslatorState.h"
#include <list>

class Retranslator : public ClientRetranslator, public DownloadRetranslator {
    friend class DirectRetranslator;
    friend class CacheRetranslator;
private:
    RetranslatorState *state;
    std::list<ClientListener*> _clients;
    void change_state(RetranslatorState *state);
    void delete_client(ClientListener *client_listener);
public:
    Retranslator();
    
    void add_client(ClientListener *client_listener);

    virtual void client_finished();
    virtual void download_add_data(const Buffer *b);
    virtual void download_finished();
    virtual void download_connect_failed();
    virtual void download_send_failed();
    virtual void download_recv_failed();
};