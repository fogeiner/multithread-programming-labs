#include "Downloader.h"
#include "DownloaderState.h"

#include "../../libs/Logger/Logger.h"
#include "../../libs/HTTPURIParser/HTTPURIParser.h"
#include "../../libs/Buffer/VectorBuffer.h"

#include "../config.h"

void Downloader::change_state(DownloaderState* s) {
    this->_state = s;
}

Downloader::Downloader() {

}

Downloader::~Downloader() {
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
