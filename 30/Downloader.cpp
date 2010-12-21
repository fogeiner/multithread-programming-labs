#include "Downloader.h"
#include "CacheEntry.h"
#include "../libs/Thread/Thread.h"
#include "Cache.h"

Downloader::Downloader(CacheEntry* cache_entry) :
_in(new VectorBuffer()),
_out(new VectorBuffer()),
_ce(cache_entry),
_sock(new TCPSocket) {
}

Downloader::~Downloader() {
    delete _in;
    delete _out;
    delete _sock;
}

void *Downloader::run(void* downloader_ptr) {
    Downloader *d = static_cast<Downloader*> (downloader_ptr);
    CacheEntry *ce = d->_ce;
    Buffer *in = d->_in;
    Buffer *out = d->_out;
    TCPSocket *sock = d->_sock;

    ce->set_downloader(d);
    const BrokenUpHTTPRequest &request = ce->request();

    // forming query to send
    out->append(ce->request().request().c_str(), ce->request().request().size());


    // try to connect
    try {
        Logger::debug("Downloader connecting to %s:%d", request.host().c_str(), request.port());
        sock->connect(request.host(), request.port());
        // if fail -- set Entry status to CON_ERROR
        // and quit
    } catch (ConnectException &ex) {
        Logger::error("Downloader ConnectException: %s", ex.what());
        Cache::drop(request.url());
        ce->lock();
        ce->remove_downloader();
        ce->set_state(CacheEntry::CONNECTION_ERROR);
        ce->broadcast();
        ce->unlock();
        sock->close();
        delete d;
        Thread::exit(NULL);
    } catch (DNSException &ex) {
        Logger::error("Downloader DNSException: %s", ex.what());
        Cache::drop(request.url());
        ce->lock();
        ce->remove_downloader();
        ce->set_state(CacheEntry::CONNECTION_ERROR);
        ce->broadcast();
        ce->unlock();
        sock->close();
        delete d;
        Thread::exit(NULL);
    }


    // try to send request
    try {
        Logger::debug("Downloader sending request");
        sock->send(out, true);
        // if fail -- set Entry status to SEND_ERROR
        // and quit
    } catch (SendException &ex) {
        Logger::error("Downloader SendException: %s", ex.what());
        ce->lock();
        ce->remove_downloader();
        ce->set_state(CacheEntry::SEND_ERROR);
        ce->broadcast();
        ce->unlock();
        delete d;
        Thread::exit(NULL);
    }

    bool response_code_got = false;

    //   while (1) {

    //   }

    // reading request from server
    // as soon as there enough symbols to detect
    // HTTP code, parse header and in case
    // it's not 200, delete CacheEntry from Cache

    // if recv fails delete Entry from Cache (if it wasn't deleted earlier)
    // set Entry status to RECV_ERROR and quit

    // if recv = 0 then set Entry to Cached one (or just FINISHED)
    // and quit

}
