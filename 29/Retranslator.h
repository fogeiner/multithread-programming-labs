#pragma once
#include <list>
class Client;
class Downloader;
class Buffer;

class Retranslator {
private:
    Downloader *_d;
    std::list<Client*> _c;
    bool _finished;
public:

    Retranslator(Downloader *downloader, std::list<Client*> clients, Buffer *buffer);

    void download_finished();

    bool is_download_finished();

    void append_data(Buffer *b);
};
