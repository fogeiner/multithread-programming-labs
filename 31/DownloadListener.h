#pragma once

class DownloadListener {
public:
    virtual void cancel() = 0;

    virtual ~DownloadListener() {
    }
};

class DummyDownloadListener : public DownloadListener {
public:

    static DownloadListener *instance() {
        static DummyDownloadListener ddl;
        return &ddl;
    }

    void cancel() {
    }
};
