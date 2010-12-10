#pragma once

class Downloader;

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

