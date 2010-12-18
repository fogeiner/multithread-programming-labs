#pragma once
#include <string>

#include "Cache/ClientRetranslator.h"
class Buffer;

class ClientListener {
public:
    virtual void add_data(const Buffer *b) = 0;
    virtual void set_retranslator(ClientRetranslator *client_retranslator) = 0;
    virtual void finished() = 0;
};
