#pragma once
#include "../../libs/AsyncDispatcher/AsyncDispatcher.h"
class Downloader;
class Retranslator;
class CacheEntry;

class DownloaderState {
public:

    virtual bool readable(const Downloader *c);

    virtual bool writable(const Downloader *c);

    virtual void handle_read(Downloader *c);

    virtual void handle_write(Downloader *c);

    virtual void handle_close(Downloader *c);

    virtual void handle_connect(Downloader *c);

protected:
    void change_state(Downloader *c, DownloaderState *s);
};

class DownloaderRequestResponse : public DownloaderState {
private:

    DownloaderRequestResponse();
public:

    static DownloaderState *instance();

    bool readable(const Downloader *d);
    bool writable(const Downloader *d);

    void handle_close(Downloader *d);

    void handle_read(Downloader *d);

    void handle_write(Downloader *d);
};

// -----------------DownloaderRetranslator---------

class DownloaderRetranslator : public DownloaderState {
private:

    DownloaderRetranslator();
public:

    static DownloaderState *instance();

    bool readable(const Downloader *d);

    bool writable(const Downloader *d);

    void handle_close(Downloader *d);

    void handle_read(Downloader *d);

};

// --------------------DownloaderCache---------------

class DownloaderCache : public DownloaderState {
private:

    DownloaderCache();
public:

    DownloaderState *instance();

    bool readable(const Downloader *d);

    bool writable(const Downloader *d);

    void handle_close(Downloader *d);

    void handle_read(Downloader *d);
};

class Downloader : public AsyncDispatcher {
    friend class DownloaderState;
    friend class DownloaderCache;
    friend class DownloaderRequestResponse;
    friend class DownloaderRetranslator;
    friend class Retranslator;
    void set_header_end_index(int index);

    void change_state(DownloaderState* s);

    void form_query();

    DownloaderState *_state;
    Buffer *_in;
    Buffer *_out;
    Retranslator *_r;
    CacheEntry *_ce;

public:

    Downloader(CacheEntry *ce);
    ~Downloader();

    bool readable() const;
    bool writable() const;
    void handle_read();
    void handle_write();
    void handle_close();
};