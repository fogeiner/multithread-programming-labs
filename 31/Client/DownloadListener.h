#pragma once

class DownloadListener {
public:
    virtual void add_data(const Buffer *b, bool absolute = false) = 0;
    virtual void finished() = 0;
    virtual void cancelled() = 0;
    virtual bool is_finished() const = 0;
    virtual bool is_cancelled() const = 0;
};