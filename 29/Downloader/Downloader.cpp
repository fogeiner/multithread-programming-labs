#include "Downloader.h"
#include "../Cache.h"
#include "../../libs/Logger/Logger.h"
#include "../../libs/HTTPURIParser/HTTPURIParser.h"
#include "../../libs/Buffer/VectorBuffer.h"
#include "../Retranslator.h"
#include "../config.h"

void Downloader::set_header_end_index(int index) {
    _ce->set_header_end_index(index);
}

void Downloader::change_state(DownloaderState* s) {
    this->_state = s;
}

void Downloader::form_query() {
    _out->append("GET ");
    _out->append(_ce->url().c_str());
    _out->append(" HTTP/1.0\r\n\r\n");
    Logger::debug("Formed request: ");
    Logger::debug(_out->buf());
}

Downloader::Downloader(CacheEntry *ce) : _in(NULL), _out(NULL), _r(NULL), _ce(ce) {

    _out = new VectorBuffer();
    _in = ce->_b;

    std::string url = ce->url();
    std::string netloc;
    short int port;

    ParsedURI *pu = HTTPURIParser::parse(url);
    netloc = pu->netloc;
    port = pu->port_n == 0 ? ProxyConfig::http_default_port : pu->port_n;
    assert(pu != NULL);
    delete pu;

    this->connect(netloc, port);
    this->form_query();
    this->_state = DownloaderRequestResponse::instance();
}

Downloader::~Downloader() {
    delete _out;
}

bool Downloader::readable() const {
    return this->_state->readable(this);
}

bool Downloader::writable() const {
    return this->_state->writable(this);
}

void Downloader::handle_read() {
    this->_state->handle_read(this);
}

void Downloader::handle_write() {
    this->_state->handle_write(this);
}

void Downloader::handle_close() {
    this->_state->handle_close(this);
}

bool DownloaderState::readable(const Downloader *c) {
    Logger::info("DownloaderState unhandled readable()");
    return false;
}

bool DownloaderState::writable(const Downloader *c) {
    Logger::info("DownloaderState unhandled writable()");
    return false;
}

void DownloaderState::handle_read(Downloader *c) {
    Logger::info("DownloaderState unhandled handle_read()");
}

void DownloaderState::handle_write(Downloader *c) {
    Logger::info("DownloaderState unhandled handle_write()");
}

void DownloaderState::handle_close(Downloader *c) {
    Logger::info("DownloaderState unhandled handle_close()");
}

void DownloaderState::handle_connect(Downloader *c) {
    Logger::info("DownloaderState unhandled handle_connect()");
}

void DownloaderState::change_state(Downloader *d, DownloaderState *s) {
    d->change_state(s);
}

DownloaderRequestResponse::DownloaderRequestResponse() {
}

DownloaderState *DownloaderRequestResponse::instance() {
    static DownloaderRequestResponse drr;
    return &drr;
}

bool DownloaderRequestResponse::readable(const Downloader *d) {
    return d->_out->size() == 0;
}

bool DownloaderRequestResponse::writable(const Downloader *d) {
    return d->_out->size() > 0;
}

void DownloaderRequestResponse::handle_close(Downloader *d) {

    Logger::debug("DownloaderRequestResponse handle_close()");
    // XXX set the status of CacheEntry
    d->close();
}

void DownloaderRequestResponse::handle_read(Downloader *d) {

    Logger::debug("DownloaderRequestResponse handle_read()");
    d->recv(d->_in);

    const char *p;
    // XXX
    if ((p = strstr(d->_in->buf(), "\r\n\r\n")) != NULL) {
        Logger::debug("Downloader found end of a response header");
        d->set_header_end_index(p - d->_in->buf() + sizeof ("\r\n\r\n"));

        // HTTP/1.x 200
        if (strstr(d->_in->buf(), "200") != d->_in->buf() + sizeof ("HTTP/1.x")) {
            Logger::debug("Response is not 200");
            // switching to retranslator mode
            d->_ce->retranslator();
        } else {
            Logger::debug("Response is 200");
            assert(false);
            // switching to cache mode
        }
    }
}

void DownloaderRequestResponse::handle_write(Downloader *d) {
    Logger::debug("DownloaderRequestResponse handle_write()");
    int sent;
    sent = d->send(d->_out);
    d->_out->drop_first(sent);
}


// -----------------DownloaderRetranslator---------

DownloaderRetranslator::DownloaderRetranslator() {
}

DownloaderState *DownloaderRetranslator::instance() {
    static DownloaderRetranslator dr;
    return &dr;
}

bool DownloaderRetranslator::readable(const Downloader *d) {
    return true;
}

bool DownloaderRetranslator::writable(const Downloader *d) {
    return false;
}

void DownloaderRetranslator::handle_close(Downloader *d) {
    d->_r->download_finished();
    d->close();
}

void DownloaderRetranslator::handle_read(Downloader *d) {
    d->_out->clear();
    d->recv(d->_out);
    d->_r->append_data(d->_out);
}


// --------------------DownloaderCache---------------

DownloaderCache::DownloaderCache() {
}

DownloaderState *DownloaderCache::instance() {
    static DownloaderCache dc;
    return &dc;
}

bool DownloaderCache::readable(const Downloader *d) {
    return true;
}

bool DownloaderCache::writable(const Downloader *d) {
    return false;
}

void DownloaderCache::handle_close(Downloader *d) {

}

void DownloaderCache::handle_read(Downloader *d) {

}
