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
    ~Retranslator();
    void remove_client(Client *c);
    void set_download_finished();
    bool is_download_finished();
    int clients_count() const;
    void forward_data_to_clients(Buffer *b);
};
