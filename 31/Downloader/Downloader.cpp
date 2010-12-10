#include "Downloader.h"
#include "DownloaderCache.h"
#include "DownloaderRequestResponse.h"
#include "DownloaderRetranslator.h"
#include "DownloaderState.h"

#include "../Cache/Cache.h"
#include "../../libs/Logger/Logger.h"
#include "../../libs/HTTPURIParser/HTTPURIParser.h"
#include "../../libs/Buffer/VectorBuffer.h"
#include "../Retranslator/Retranslator.h"
#include "../config.h"

void Downloader::change_state(DownloaderState* s) {
    this->_state = s;
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

    this->_out->append(ce->get_query());
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

void Downloader::handle_connect() {
    this->_state->handle_connect(this);
}
