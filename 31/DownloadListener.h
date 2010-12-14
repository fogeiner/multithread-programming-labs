#pragma once

class DownloadListener {
public:
    virtual void cancel() = 0;
    virtual ~DownloadListener(){}
};