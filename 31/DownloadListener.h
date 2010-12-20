#pragma once

class DownloadListener {
public:
    virtual void cancel() {};

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
