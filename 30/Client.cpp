#include "Client.h"

Client::Client(TCPSocket *c_sock) :
_sock(c_sock),
_in(new VectorBuffer()),
_out(new VectorBuffer()),
_request(""),
_bytes_sent(0),
_ce(NULL) {
    Logger::debug("Client::Client() fd=%d", _sock->fileno());
}

Client::~Client() {
    Logger::debug("Client::~Client()");
    delete _sock;
    delete _in;
    delete _out;
}

void Client::set_cache_entry(CacheEntry *cache_entry) {
    _ce = cache_entry;
}

void Client::close_delete_exit() {
    _sock->close();
    delete this;
    Thread::exit(NULL);
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

    _request = BrokenUpHTTPRequest(url, request, method, host, path, version, port);
    return true;
}

void *Client::run(void *client_ptr) {
    Client *c = static_cast<Client*> (client_ptr);
    // receiving data until \r\n\r\n
    // if client closes connection -- quit

    // parsing request
    // if it's uncorrect -- get cached error msg page
    // and send it all
    // if error -- quit

    // if request is OK then we should
    // check Cache for such entry

    try {
        do {
            try {
                if (c->_sock->recv(c->_in) == 0) {
                    Logger::debug("Client closed connection");
                    c->_sock->close();
                    delete c;
                    Thread::exit(NULL);
                }
            } catch (RecvException &ex) {
                Logger::error("Client::recv_request() RecvException");

                c->close_delete_exit();
            }
        } while (!c->parse_request());

        Logger::debug("Client request was successfully parsed");
        Cache::request(c->_request, c);
    } catch (NotImlementedException &ex) {
        Logger::error("Client::parse_request() NotImplementedException");
        Cache::request(Cache::HTTP_NOT_IMPLEMENTED, c);
    } catch (BadRequestException &ex) {
        Logger::error("Client::parse_request() BadRequestException");
        Cache::request(Cache::HTTP_BAD_REQUEST, c);
    }
    c->_in->clear();

    // in request procedure Client's _ce is meant to be set to
    // the active CacheEntry
    bool done = false;

    CacheEntry *ce;

    do {
        ce = c->_ce;

        Buffer *out = c->_out;
        CacheEntry::CacheEntryState ce_state;

        ce->lock();

        ce_state = ce->get_state();

        while ((ce_state == CacheEntry::CACHING || ce_state == CacheEntry::DOWNLOADING)
                && c->_bytes_sent >= ce->bytes_received()) {
            Logger::debug("Client waits");
            ce->wait();
            ce_state = ce->get_state();
        }

        Logger::debug("Client works");

        // copying data from CacheEntry to _out and informing CacheEntry about it
        const Buffer *cache_entry_buffer = ce->data();
        size_t client_bytes_got = c->_out->size() + c->_bytes_sent;
        size_t cache_entry_bytes_received = ce->bytes_received();
        size_t cache_entry_buffer_size = cache_entry_buffer->size();

        Buffer *sub_buf = cache_entry_buffer->subbuf(cache_entry_buffer_size -
                (cache_entry_bytes_received - client_bytes_got), cache_entry_buffer_size);

        out->append(sub_buf);

        delete sub_buf;

        ce->broadcast();
        ce->unlock();

        switch (ce_state) {
                // working; add data and try to send it
            case CacheEntry::CACHING:
            {
                Logger::debug("Client: CACHING");
                break;
            }
                // cache entry is done; add data, try to sent it and quit
            case CacheEntry::FINISHED:
            {
                Logger::debug("Client: FINISHED");
                done = true;
                break;
            }
                // working; add data, try to send it
            case CacheEntry::DOWNLOADING:
            {
                Logger::debug("Client: DOWNLOADING");
                break;
            }
                // cache entry is done, try to sent it and quit
            case CacheEntry::CACHED:
            {
                Logger::debug("Client: CACHED");
                done = true;
                break;
            }
                // there was a problem while receiving response from the server
                // send whatever available and quit
            case CacheEntry::RECV_ERROR:
            {
                Logger::debug("Client: RECV_ERROR");
                done = true;
                break;
            }
                // there was a problem during connection (host not found?)
                // cache cache entry for another and try again
            case CacheEntry::CONNECTION_ERROR:
            {
                Logger::debug("Client: CONNECTION_ERROR");
                c->_out->clear();
                c->_bytes_sent = 0;
                Cache::request(Cache::HTTP_SERVICE_UNAVAILABLE, c);
                break;
            }
                // there was a problem while sending request to the server
                // send cache entry for another and try again
            case CacheEntry::SEND_ERROR:
            {
                Logger::debug("Client: SEND_ERROR");
                c->_out->clear();
                c->_bytes_sent = 0;
                Cache::request(Cache::HTTP_INTERNAL_ERROR, c);
                break;
            }
        }


        try {
            int sent = c->_sock->send(c->_out, true);
            c->_bytes_sent += sent;
            c->_out->drop_first(sent);

            ce->lock();
            ce->data_got(c->_bytes_sent, c);
            ce->broadcast();
            ce->unlock();
        } catch (SendException &ex) {
            Logger::error("Client sending data: %s", ex.what());
            done = true;
        }
    } while (!done);

    ce->lock();
    ce->remove_client(c);

    if (ce->to_delete()) {
        ce->unlock();
        delete ce;
    } else {
        ce->broadcast();
        ce->unlock();
    }

    c->close_delete_exit();
}
