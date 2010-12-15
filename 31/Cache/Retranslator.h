#pragma once
#include "../ClientListener.h"
#include "../BrokenUpHTTPRequest.h"
#include "CacheEntry.h"
#include "ClientRetranslator.h"
#include "DownloadRetranslator.h"
#include "RetranslatorState.h"
#include <list>

class Retranslator : public ClientRetranslator, public DownloadRetranslator {
    friend class DirectRetranslator;
    friend class CacheRetranslator;
private:
    RetranslatorState *_state;
    std::list<ClientListener*> _clients;
    void change_state(RetranslatorState *state);
    void delete_client(ClientListener *client_listener);
    int clients_count() const;
    DownloadListener *_download_listener;
    bool _response_code_received;
    const BrokenUpHTTPRequest _request;
    CacheEntry &_ce;
public:
    Retranslator(const BrokenUpHTTPRequest request, CacheEntry &cache_entry, ClientListener *initial_client_listener);

    virtual void add_client(ClientListener *client_listener);
    virtual void client_finished(ClientListener *client_listener);
    virtual void download_add_data(const Buffer *b);
    virtual void download_finished();
    virtual void download_connect_failed();
    virtual void download_send_failed();
    virtual void download_recv_failed();
    virtual ~Retranslator(){}
};
