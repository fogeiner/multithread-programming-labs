#include "Client.h"

#include "../Retranslator.h"
#include "../config.h"
#include "../../libs/Logger/Logger.h"
#include "../../libs/Buffer/VectorBuffer.h"
#include "../Cache.h"
void Client::change_state(ClientState* s) {
    this->_state = s;
}

Client::Client(TCPSocket *sock) : AsyncDispatcher(sock) {
    this->_state = ClientGettingRequest::instance();
    this->_bytes_sent = 0;
    this->_b = new VectorBuffer();
}

void Client::error(const char *msg) {
    this->error(std::string(msg));
}

void Client::error(std::string msg) {
    _b->clear();
    _b->append(msg.c_str());
    this->change_state(ClientError::instance());
}

bool Client::readable() const {
    return this->_state->readable(this);
}

bool Client::writable() const {
    return this->_state->writable(this);
}

void Client::handle_read() {
    this->_state->handle_read(this);
}

void Client::handle_write() {
    this->_state->handle_write(this);
}

void Client::handle_close() {
    this->_state->handle_close(this);
}




bool ClientState::readable(const Client *c) {
    Logger::info("ClientState unhandled readable()");
    return false;
}

bool ClientState::writable(const Client *c) {
    Logger::info("ClientState unhandled writable()");
    return false;
}

void ClientState::handle_read(Client *c) {
    Logger::info("ClientState unhandled handle_read()");
}

void ClientState::handle_write(Client *c) {
    Logger::info("ClientState unhandled handle_write()");
}

void ClientState::handle_close(Client *c) {
    Logger::info("ClientState unhandled handle_close()");
}

void ClientState::handle_connect(Client *c) {
    Logger::info("ClientState unhandled handle_connect()");
}

void ClientState::change_state(Client *c, ClientState *s) {
    c->change_state(s);
}
ClientError::ClientError() {
}

ClientState *ClientError::instance() {
    static ClientError cbr;
    return &cbr;
}

bool ClientError::readable(const Client *c) {
    return false;
}
bool ClientError::writable(const Client *c) {
    return c->_b->size() >= 0;
}

void ClientError::handle_write(Client *c) {
    try {
        int sent = c->send(c->_b);
        c->_b->drop_first(sent);
        if (c->_b->size() == 0) {
            c->close();
        }
    } catch (SendException &ex) {
        c->close();
    }
}

// ------------------ClientGettingRequest---------------------------

ClientGettingRequest::ClientGettingRequest() {
}

ClientState *ClientGettingRequest::instance() {
    static ClientGettingRequest cgr;
    return &cgr;
}

bool ClientGettingRequest::readable(const Client *c) {
    return true;
}

bool ClientGettingRequest::writable(const Client *c) {
    return false;
}

class ClientBadRequestException : public std::exception {
};

void ClientGettingRequest::handle_read(Client *c) {
    Logger::debug("ClientGettingRequest::handle_read()");
    try {
        c->recv(c->_b);

        // checking if there's \r\n\r\n in request so far
        const char *b = c->_b->buf();
        const char *p;
        p = strstr(b, "\r\n\r\n");

        // there's \r\n\r\n in the buffer; time to parse!
        if (p != NULL) {
            Logger::debug("http_delimiter found; parsing request");

            try {
                // firstly we shall copy the first line (if there are many)
                // it ends with \r\n
                p = strstr(b, "\r\n");

                // smallest query is GET / HTTP1.x which is 14 with \0 at the end
                // btw, it's incorrect for proxy as it should contain full uri
                if (p - b + 1 < 14) {
                    // going to ClientBadRequestState
                    throw ClientBadRequestException();
                }

                char *query = (char*) alloca(p - b + 1);
                for (int i = 0; i < p - b; ++i) {
                    query[i] = b[i];
                }
                query[p - b] = '\0';

#ifdef D
                fprintf(stderr, "query: %s\n", query);
#endif

                c->_m = Client::GET;
                // we have to support only HEAD and GET
                // (GET|HEAD) URL HTTP/1.X
                p = strstr(query, "GET");
                if (p == NULL || p != query) {
                    p = strstr(query, "HEAD");
                    c->_m = Client::HEAD;
                }
                if (p == NULL || p != query) {
                    // it's not a query that we support; going to ClientBadRequestState
                    throw ClientBadRequestException();
                }

                // now we have to get URL
                // it starts with http and ends
                const char *http_start, *url_end;
                http_start = strstr(query, "http://");
                if (http_start == NULL) {
                    throw ClientBadRequestException();
                }
                url_end = strstr(http_start, " ");
                if (url_end == NULL) {
                    throw ClientBadRequestException();
                }

                char *url = (char*) alloca(url_end - http_start + 1);
                for (int i = 0; i < url_end - http_start; ++i) {
                    url[i] = http_start[i];
                }
                url[url_end - http_start] = '\0';
#ifdef D
                fprintf(stderr, "URL: %s\n", url);
#endif
                // XXX
                Cache *cache = Cache::instance();
                CacheEntry *ce;
                // no such entry in cache
                if ((ce = cache->get(url)) == NULL) {
                    Logger::debug("No cache entry for the URL found");
                    ce = new CacheEntry(std::string(url));
                    ce->add_client(c);
                    cache->add(std::string(url), ce);
                    c->change_state(ClientCache::instance());
                    ce->activate();
                } else {
                    assert(false); // for now shouldn't come here
                    Logger::debug("Cache entry for URL found");
                    ce->add_client(c);
                }

            } catch (ClientBadRequestException &ex) {
                Logger::debug("Bad request found");
                c->error(ProxyConfig::bad_request_msg);
            }

        }
    } catch (RecvException &ex) {
        Logger::error(ex.what());
    }
}

void ClientGettingRequest::handle_close(Client *c) {
    delete c->_b;
    c->close();
}
// ------------------ClientRetranslator--------------

ClientRetranslator::ClientRetranslator() {
}

ClientState *ClientRetranslator::instance() {
    static ClientRetranslator cr;
    return &cr;
}

bool ClientRetranslator::writable(const Client *c) {
    return c->_b->size() > 0;
}

void ClientRetranslator::handle_write(Client *c) {
    Logger::debug("Retranslating to client");
    int sent;
    sent = c->send(c->_b);
    c->_b->drop_first(sent);

    if (c->_r->is_download_finished() && c->_b->size() == 0) {
        Logger::debug("Retranslating to client finished");
        c->close();
    }
}


// ------------------ClientCache---------------------

ClientCache::ClientCache() {
}

ClientState *ClientCache::instance() {
    static ClientCache cc;
    return &cc;
}

bool ClientCache::readable(const Client *c) {
    return false;
}

bool ClientCache::writable(const Client *c) {
    return true; // XXX true!

}

void ClientCache::handle_write(Client *c) {
}
