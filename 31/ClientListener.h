#pragma once
#include <string>

#include "Cache/ClientRetranslator.h"
class Buffer;

class ClientListener {
public:
    virtual void add_data(const Buffer *b) {};
    virtual void set_retranslator(ClientRetranslator *client_retranslator) {};
    virtual void finished() {};
};

class DummyClientListener : public ClientListener {
public:

    static ClientListener *instance() {
        static DummyClientListener dcl;
        return &dcl;
    }

    void add_data(const Buffer *b) {
    }

    void set_retranslator(ClientRetranslator *client_retranslator) {
    }

    void finished() {
    }
};
