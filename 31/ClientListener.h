#pragma once
#include <string>
class Buffer;

class ClientListener {
public:
    virtual void add_data(const Buffer *b, bool no_reply = false) = 0;
    virtual void finished(bool no_reply = false) = 0;
};