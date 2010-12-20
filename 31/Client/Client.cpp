#include "Client.h"

#include "../../libs/Logger/Logger.h"
#include "../../libs/HTTPURIParser/HTTPURIParser.h"
#include "../Cache/Cache.h"
#include "../BrokenUpHTTPRequest.h"
#include "../config.h"
#include <string>
#include <sstream>

Client::Client(TCPSocket *sock) : AsyncDispatcher(sock),
_client_retranslator(DummyRetranslator::instance()),
_got_request(false),
_bytes_sent(0),
_finished(false),
_out_mutex(Mutex::RECURSIVE_MUTEX),
_callback_mutex(Mutex::RECURSIVE_MUTEX) {
    _in = new VectorBuffer();
    _out = new VectorBuffer();
    this->activate();
}

Client::~Client() {
    Logger::debug("Client::~Client(); locks_count %d", _out_mutex.locks_count());
    delete _in;
    delete _out;
}

bool Client::readable() const {
    return _got_request == false;
}

bool Client::writable() const {

    if (_got_request == false) {
        return false;
    }
    _out_mutex.lock();
    bool ret = _finished || _out->size() > 0;
    _out_mutex.unlock();

    return ret;
}

class NotImlementedException {
};

class BadRequestException {
};

void Client::handle_read() {
    Logger::debug("Client::handle_read()");
    try {
        recv(_in);
        std::string raw_request = *_in;

        size_t pos;
        if ((pos = raw_request.find("\r\n\r\n")) == std::string::npos) {
            return;
        }
        //    Logger::debug("\\r\\n\\r\\n found at the position %ld", pos);

        try {
            std::string request = raw_request;
            std::string method;
            std::string host;
            std::string path;
            std::string version;
            std::string url;
            unsigned short port;

            std::string word;
            std::istringstream iss(raw_request, std::istringstream::in);

            iss >> word;
            if (word != "GET") {
                throw NotImlementedException();
            }
            method = word;

            iss >> word;
            ParsedURI *pu = HTTPURIParser::parse(word);
            if (pu == NULL) {
                throw BadRequestException();
            }
            url = word;

            port = pu->port_n == 0 ? 80 : pu->port_n;
            host = pu->netloc;
            path = pu->path;
            delete pu;

            iss >> word;
            if (word != "HTTP/1.1" && word != "HTTP/1.0") {
                throw BadRequestException();
            }
            version = word;

            request.erase(request.end() - 2, request.end());
            request.append("Connection: close\r\n\r\n");

            Logger::debug("Client request: %s", ("http://" + host + path).c_str());

            //      Logger::debug((request + method + host + path + url).c_str());
            request.erase(0, request.find("\r\n"));
            request.insert(0, (method + " " + path + " " + version));
            BrokenUpHTTPRequest broken_up_request(url, request, method, host, path, version, port);

            Cache::request(broken_up_request, this);

        } catch (NotImlementedException &ex) {
            Logger::error("Client::handle_read() NotImplementedException");
            BrokenUpHTTPRequest broken_up_request(Cache::HTTP_NOT_IMPLEMENTED);

            Cache::request(broken_up_request, this);

        } catch (BadRequestException &ex) {
            Logger::error("Client::handle_read() BadRequestException");

            BrokenUpHTTPRequest broken_up_request(Cache::HTTP_BAD_REQUEST);

            Cache::request(broken_up_request, this);
        }
    } catch (RecvException &ex) {
        Logger::error("Client::handle_read() RecvException");
        _callback_mutex.lock();
        _client_retranslator->client_finished(this);
        this->finished();
        _callback_mutex.unlock();
        close();
    }


    _got_request = true;
}

void Client::handle_write() {
    Logger::debug("Client::handle_write()");

    try {
        _out_mutex.lock();

        int sent = send(_out);
        _bytes_sent += sent;

        _out->drop_first(sent);
        if (_finished && (_out->size() == 0)) {
            _out_mutex.unlock();
            _callback_mutex.lock();
            _client_retranslator->client_finished(this);
            this->finished();
            _callback_mutex.unlock();
            close();

            return;
        }
        _out_mutex.unlock();
    } catch (SendException &ex) {
        Logger::debug("Client::handle_write() SendException");
        _out_mutex.unlock();
        _callback_mutex.lock();
        _client_retranslator->client_finished(this);
        this->finished();
        _callback_mutex.unlock();
        close();
    }

}

void Client::handle_close() {
    Logger::debug("Client::handle_close()");
    _callback_mutex.lock();
    _client_retranslator->client_finished(this);
    this->finished();
    _callback_mutex.unlock();
    close();
}

void Client::add_data(const Buffer *b) {
    if (_finished) {
        return;
    }

    _out_mutex.lock();

    Logger::debug("Client::add_data(%p)", b);
    _out->append(b);
    _out_mutex.unlock();
}

void Client::finished() {
    Logger::debug("Client::finished()");
    //_callback_mutex.lock();
    this->set_retranslator(DummyRetranslator::instance());
    _finished = true;
   // _callback_mutex.unlock();
}

void Client::set_retranslator(ClientRetranslator* client_retranslator) {
    Logger::debug("Client::set_retranslator(%p)", client_retranslator);
   // _callback_mutex.lock();
    if (!_finished) {
        _client_retranslator = client_retranslator;
    } else {
        Logger::debug("Client::set_retranslator() ignoring because _finished");
    }
    //_callback_mutex.unlock();
}
