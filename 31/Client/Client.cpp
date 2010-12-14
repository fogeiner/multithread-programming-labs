#include "Client.h"

#include "../../libs/Logger/Logger.h"
#include "../../libs/HTTPURIParser/HTTPURIParser.h"
#include "../Cache/Cache.h"
#include "../BrokenUpHTTPRequest.h"
#include "../config.h"
#include <string>
#include <sstream>

Client::Client(TCPSocket *sock) : AsyncDispatcher(sock),
_got_request(false),
_no_reply(false),
_bytes_sent(0),
_finished(false),
_mutex(Mutex::ERRORCHECK_MUTEX) {
    _in = new VectorBuffer();
    _out = new VectorBuffer();
    this->activate();
}

Client::~Client() {
    delete _in;
    delete _out;
}

bool Client::readable() const {
    return _got_request == false;
}

bool Client::writable() const {
    return (_got_request == true) && (_out->size() > 0 || _finished);
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

        _got_request = true;

        //    Logger::debug("\\r\\n\\r\\n found at the position %ld", pos);

        try {
            std::string request = raw_request;
            std::string method;
            std::string host;
            std::string path;
            unsigned short port;
            std::string url;

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

            request.erase(request.end() - 2, request.end());
            request.append("Connection: close\r\n\r\n");

            Logger::debug("Client request: %s", ("http://" + host + path).c_str());

            //      Logger::debug((request + method + host + path + url).c_str());
            BrokenUpHTTPRequest broken_up_request(url, request, method, host, path, port);


            _client_retranslator = Cache::request(broken_up_request, this);

        } catch (NotImlementedException &ex) {
            Logger::error("Client::handle_read() NotImplementedException");
            BrokenUpHTTPRequest broken_up_request(Cache::HTTP_NOT_IMPLEMENTED);
            _client_retranslator = Cache::request(broken_up_request, this);
        } catch (BadRequestException &ex) {
            Logger::error("Client::handle_read() BadRequestException");
            BrokenUpHTTPRequest broken_up_request(Cache::HTTP_BAD_REQUEST);
            _client_retranslator = Cache::request(broken_up_request, this);
        }
    } catch (RecvException &ex) {
        Logger::error("Client::handle_read() RecvException");
        if (!_no_reply && _client_retranslator == NULL) {
            _client_retranslator->client_finished(this);
        }
        close();
    }
}

void Client::handle_write() {
    Logger::debug("Client::handle_write()");
    try {
        int sent = send(_out);
        _bytes_sent += sent;

        _mutex.lock();
        _out->drop_first(sent);
        _mutex.unlock();

        if (_finished && _out->size() == 0) {
            if (!_no_reply && _client_retranslator != NULL) {
                _client_retranslator->client_finished(this);
            }
            close();
        }

    } catch (SendException &ex) {
        Logger::debug("Client::handle_write() SendException");
        if (!_no_reply && _client_retranslator != NULL) {
            _client_retranslator->client_finished(this);
        }
        close();
    }
}

void Client::handle_close() {
    Logger::debug("Client::handle_close()");
    if (!_no_reply && _client_retranslator != NULL) {
        _client_retranslator->client_finished(this);
    }
    close();
}

void Client::add_data(const Buffer *b, bool no_reply) {
    _mutex.lock();
    _no_reply = no_reply;
    Logger::debug("Client::add_data(%p)", b);
    _out->append(b);
    _mutex.unlock();
}

void Client::finished(bool no_reply) {
    Logger::debug("Client::finished()");
    _no_reply = no_reply;
    _finished = true;
}
