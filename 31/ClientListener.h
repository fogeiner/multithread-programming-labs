#pragma once
#include <string>
class Buffer;

class ClientListener {
public:
    virtual void add_data(const Buffer *b) = 0;
    virtual void finished() = 0;
    virtual bool is_finished() const = 0;
};