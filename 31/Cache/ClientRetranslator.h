#pragma once

class ClientListener;
class ClientRetranslator {
public:
    virtual void client_finished(ClientListener *client_listener) {}
    virtual ~ClientRetranslator(){}
};
