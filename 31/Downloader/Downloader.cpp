#include "Downloader.h"

#include "../../libs/Logger/Logger.h"
#include "../../libs/HTTPURIParser/HTTPURIParser.h"
#include "../../libs/Buffer/VectorBuffer.h"

#include "../config.h"

Downloader::Downloader(BrokenUpHTTPRequest request, DownloadRetranslator *download_retranslator) :
_download_retranslator(download_retranslator),
_cancelled(false),
_mutex(Mutex::RECURSIVE_MUTEX) {
    _in = new VectorBuffer();
    _out = new VectorBuffer();
    _out->append(request.request.c_str(), request.request.size());

    try {
        this->connect(request.host, request.port);
        this->activate();
    } catch (DNSException &ex) {
        _download_retranslator->download_connect_failed();
        close();
    }
}

Downloader::~Downloader() {
    delete _in;
    delete _out;
}

bool Downloader::readable() const {
    return _out->size() == 0;
}

bool Downloader::writable() const {
    return _out->size() > 0;
}

void Downloader::handle_read() {
    _mutex.lock();
    if (_cancelled) {
        _mutex.unlock();
        _cancel();
        return;
    }
    Logger::debug("Downloader::handle_read()");
    try {
        recv(_in);
        _download_retranslator->download_add_data(_in);
        _in->clear();
    } catch (RecvException &ex) {
        Logger::debug("Downloader::handle_write() RecvException: %s", ex.what());
        _download_retranslator->download_recv_failed();
        close();
    }
    _mutex.unlock();
}

void Downloader::handle_write() {
    _mutex.lock();
    if (_cancelled) {
        _mutex.unlock();
        _cancel();
        return;
    }
    Logger::debug("Downloader::handle_write()");
    try {
        _out->drop_first(send(_out));
    } catch (SendException &ex) {
        Logger::debug("Downloader::handle_write() SendException: %s", ex.what());
        _download_retranslator->download_send_failed();
        close();
    }
    _mutex.unlock();
}

void Downloader::handle_close() {
    Logger::debug("Downloader::handle_close()");
    _mutex.lock();
    if (_cancelled == false) {
        _download_retranslator->download_finished();
    }
    _mutex.unlock();
    close();
}

void Downloader::handle_connect() {
    _mutex.lock();
    if (_cancelled) {
        _mutex.unlock();
        _cancel();
        return;
    }

    Logger::debug("Downloader::handle_connect()");
    try {
        validate_connect();
    } catch (ConnectException &ex) {
        Logger::debug("Downloader::handle_read() ConnectException: %s", ex.what());
        _download_retranslator->download_connect_failed();
        close();
    }
    _mutex.unlock();
}

void Downloader::cancel() {
    _mutex.lock();
    _cancelled = true;
    _mutex.unlock();
}

void Downloader::_cancel() {
    Logger::debug("Downloader::_cancel()");
    close();
}