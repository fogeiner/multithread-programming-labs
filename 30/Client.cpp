#include "Client.h"

Client::Client(TCPSocket *c_sock) :
_sock(c_sock),
_in(new VectorBuffer()),
_out(new VectorBuffer()),
_request(NULL),
_bytes_sent(0),
_ce(NULL) {
    Logger::debug("Client::Client() fd=%d", _sock->fileno());
}

Client::~Client() {
    Logger::debug("Client::~Client()");
    delete _in;
    delete _out;
    delete _request;
}

void Client::set_cache_entry(CacheEntry *cache_entry) {
    _ce = cache_entry;
}

bool Client::parse_request() {
    std::string raw_request = *_in;

    size_t pos;
    if ((pos = raw_request.find("\r\n\r\n")) == std::string::npos) {
        return false;
    }

    Logger::debug("\\r\\n\\r\\n found at the position %ld", pos);


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

    request.erase(0, request.find("\r\n"));
    request.insert(0, (method + " " + path + " " + version));

    _request = new BrokenUpHTTPRequest(url, request, method, host, path, version, port);
}

void *Client::run(void *client_ptr) {
    Client *c = static_cast<Client*> (client_ptr);
    // receiving data until \r\n\r\n
    // if client closes connection -- quit

    // parsing request
    // if it's uncorrect -- get cached error msg page
    // and send it all
    // if error -- quit

    try {
        do {
            try {
                if (c->_sock->recv(c->_in) == 0) {
                    c->_sock->close();
                    delete c;
                    return NULL;
                }
            } catch (RecvException &ex) {
                Logger::error("Client::recv_request() RecvException");
                c->_sock->close();
                delete c;
                return NULL;
            }
        } while (!c->parse_request());

        Cache::request(*c->_request, c);
    } catch (NotImlementedException &ex) {
        Logger::error("Client::parse_request() NotImplementedException");
        BrokenUpHTTPRequest broken_up_request(Cache::HTTP_NOT_IMPLEMENTED);
        Cache::request(broken_up_request, c);
    } catch (BadRequestException &ex) {
        Logger::error("Client::parse_request() BadRequestException");
        BrokenUpHTTPRequest broken_up_request(Cache::HTTP_BAD_REQUEST);
        Cache::request(broken_up_request, c);
    }

    // if request is OK then we should
    // check Cache for such entry

    // in case there's one and CACHED then we just
    // copy contents to the _out and send_it_all

    // in case there's one and it's CACHING then we just
    // add this client to than Entry

    // in case there's no such Entry, we create one;
    // Entry is meant to download data by herself

    // come to producer-consumer loop with CV and mutex synchronization
    // until Entry is Cached or damaged
    // checking flag of Entry {CACHING, CACHED, CONN_ERROR, SEND_ERROR, RECV_ERROR}
    //

}
