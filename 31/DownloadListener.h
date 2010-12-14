#pragma once
#include <string>
class Buffer;
class DownloadListener {
public:
    virtual void add_data(std::string key, const Buffer *b) = 0;
    virtual void finished() = 0;
    virtual void cancelled() = 0;
    virtual bool is_finished() const = 0;
    virtual bool is_cancelled() const = 0;
};