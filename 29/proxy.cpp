#include "../libs/Logger/Logger.h"
#include "../libs/AsyncDispatcher/AsyncDispatcher.h"
#include "../libs/Buffer/VectorBuffer.h"
#include "../libs/HTTPURIParser/HTTPURIParser.h"

#include <exception>
#include <cstring>
#include <alloca.h>

#include <list>
#include <map>
#include <cassert>

#include "config.h"

#define D

#ifdef D
#include <cstdio>
#endif

class Retranslator;

// -------------------ClientState------------------------------
class Client;

class ClientState {
public:

    virtual bool readable(const Client *c) {
        Logger::info("ClientState unhandled readable()");
        return false;
    }

    virtual bool writable(const Client *c) {
        Logger::info("ClientState unhandled writable()");
        return false;
    }

    virtual void handle_read(Client *c) {
        Logger::info("ClientState unhandled handle_read()");
    }

    virtual void handle_write(Client *c) {
        Logger::info("ClientState unhandled handle_write()");
    }

    virtual void handle_close(Client *c) {
        Logger::info("ClientState unhandled handle_close()");
    }

    virtual void handle_connect(Client *c) {
        Logger::info("ClientState unhandled handle_connect()");
    }

protected:
    void change_state(Client *c, ClientState *s);
};

// -------------------DownloaderState----------------------
class Downloader;

class DownloaderState {
public:

    virtual bool readable(const Downloader *c) {
        Logger::info("DownloaderState unhandled readable()");
        return false;
    }

    virtual bool writable(const Downloader *c) {
        Logger::info("DownloaderState unhandled writable()");
        return false;
    }

    virtual void handle_read(Downloader *c) {
        Logger::info("DownloaderState unhandled handle_read()");
    }

    virtual void handle_write(Downloader *c) {
        Logger::info("DownloaderState unhandled handle_write()");
    }

    virtual void handle_close(Downloader *c) {
        Logger::info("DownloaderState unhandled handle_close()");
    }

    virtual void handle_connect(Downloader *c) {
        Logger::info("DownloaderState unhandled handle_connect()");
    }

protected:
    void change_state(Downloader *c, DownloaderState *s);
};

// ----------------------------CacheEntry-----------------

class CacheEntry {
    friend class Client;
    friend class Downloader;
private:

    std::list<Client*> &clients() {
        return _c;
    }
    Downloader *_d;
    std::list<Client*> _c;
    Buffer *_b;
    int _header_end_index;
    std::string _url;
public:

    CacheEntry(std::string url) : _url(url) {
    }

    void activate();

    void add_client(Client *c) {
        _c.push_back(c);
    }

    void remove_client(Client *c) {
        _c.remove(c);
    }

    std::string url() const {
        return _url;
    }
};

// ----------------------Cache--------------------------------

class Cache {
private:

    Cache() {
    }
    static std::map<std::string, CacheEntry*> _c;
public:

    static Cache *instance() {
        static Cache c;
        return &c;
    }

    static CacheEntry *get(std::string key) {
        std::map<std::string, CacheEntry*>::iterator iter = _c.find(key);
        if (iter != _c.end()) {
            return iter->second;
        } else {
            return NULL;
        }
    }

    static void add(std::string key, CacheEntry *ce) {
        assert(ce != NULL);
        _c.insert(std::pair<std::string, CacheEntry*>(key, ce));
    }

    static void remove(std::string key) {
        std::map<std::string, CacheEntry*>::iterator iter = _c.find(key);
        assert(iter != _c.end());
        _c.erase(iter);
    }
};

std::map<std::string, CacheEntry*> Cache::_c;

// -----------------------Client--------------------------------------

class Client : public AsyncDispatcher {
public:

    enum method {
        HEAD, GET
    };

private:
    friend class ClientState;
    friend class ClientGettingRequest;
    friend class ClientError;
    friend class ClientCache;
    friend class ClientRetranslator;

    void change_state(ClientState* s) {
        this->_state = s;
    }
    ClientState *_state;
    int _bytes_sent;
    Buffer *_b;
    Retranslator *_r;
    CacheEntry *_ce;
    enum method _m;
public:


    Client(TCPSocket *sock);

    void error(const char *msg);

    void error(std::string msg);

    bool readable() const {
        return this->_state->readable(this);
    }

    bool writable() const {
        return this->_state->writable(this);
    }

    void handle_read() {
        this->_state->handle_read(this);
    }

    void handle_write() {
        this->_state->handle_write(this);
    }

    void handle_close() {
        this->_state->handle_close(this);
    }
};

// --------------------DownloaderRequestResponse---------------

class DownloaderRequestResponse : public DownloaderState {
private:

    DownloaderRequestResponse() {
    }
public:

    static DownloaderState *instance() {
        static DownloaderRequestResponse drr;
        return &drr;
    }

    bool readable(const Downloader *d);
    bool writable(const Downloader *d);

    void handle_close(Downloader *d);

    void handle_read(Downloader *d);

    void handle_write(Downloader *d);
};


// ------------------Downloader---------------------------
class Retranslator;

class Downloader : public AsyncDispatcher {
    friend class DownloaderState;
    friend class DownloaderCache;
    friend class DownloaderRequestResponse;
    friend class DownloaderRetranslator;

    void change_state(DownloaderState* s) {
        this->_state = s;
    }

    void form_query() {
        _out->append("GET ");
        _out->append(_ce->url().c_str());
        _out->append(" HTTP/1.0\r\n\r\n");
        Logger::debug("Formed request: ");
        Logger::debug(_out->buf());
    }

    DownloaderState *_state;
    Buffer *_in;
    Buffer *_out;
    Retranslator *_r;
    CacheEntry *_ce;

public:

    Downloader(CacheEntry *ce) : _in(NULL), _out(NULL), _r(NULL), _ce(ce) {

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

    ~Downloader() {
        delete _out;
    }

    bool readable() const {
        return this->_state->readable(this);
    }

    bool writable() const {
        return this->_state->writable(this);
    }

    void handle_read() {
        this->_state->handle_read(this);
    }

    void handle_write() {
        this->_state->handle_write(this);
    }

    void handle_close() {
        this->_state->handle_close(this);
    }
};


// --------------------DownloaderRequestResponse---------------

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

    // XXX
    if (strstr(d->_in->buf(), "\r\n\r\n") != NULL) {
        Logger::debug("Downloader found end of a response header");

        // HTTP/1.x 200
        if (strstr(d->_in->buf(), "200") != d->_in->buf() + sizeof ("HTTP/1.x ")) {
            Logger::debug("Response is not 200");

        } else {
            Logger::debug("Response is 200");
        }
    } 
}

void DownloaderRequestResponse::handle_write(Downloader *d) {
    Logger::debug("DownloaderRequestResponse handle_write()");
    int sent;
    sent = d->send(d->_out);
    d->_out->drop_first(sent);
}


// ----------------------CacheEntry--------------------------

void CacheEntry::activate() {
    try {
        _d = new Downloader(this);
    } catch (DNSException &ex) {
        Logger::debug("Requested server not found");
        for (std::list<Client*>::iterator i = _c.begin();
                i != _c.end(); ++i) {
            Logger::debug("Deleting CacheEntry clients");
            Client *c = *i;
            c->error(ProxyConfig::server_not_found_msg);

        }
        Cache::instance()->remove(_url);
    }
}

// -----------------DownloaderRetranslator---------

class DownloaderRetranslator : public DownloaderState {
private:

    DownloaderRetranslator() {
    }
public:

    DownloaderState *instance() {
        static DownloaderRetranslator dr;
        return &dr;
    }

    bool readable(const Downloader *d) {
        return true;
    }

    bool writable(const Downloader *d) {
        return false;
    }

    void handle_close(Downloader *d) {

    }

    void handle_read(Downloader *d) {

    }

};

// --------------------DownloaderCache---------------

class DownloaderCache : public DownloaderState {
private:

    DownloaderCache() {
    }
public:

    DownloaderState *instance() {
        static DownloaderCache dc;
        return &dc;
    }

    bool readable(const Downloader *d) {
        return true;
    }

    bool writable(const Downloader *d) {
        return false;
    }

    void handle_close(Downloader *d) {

    }

    void handle_read(Downloader *d) {

    }
};

// ----------------ClientError-----------------------

class ClientError : public ClientState {

    ClientError() {
    }
public:

    static ClientState *instance() {
        static ClientError cbr;
        return &cbr;
    }

    virtual bool readable(const Client *c) {
        return false;
    }

    virtual bool writable(const Client *c);
    virtual void handle_write(Client *c);
};

// ------------------ClientGettingRequest---------------------------

class ClientGettingRequest : public ClientState {

    ClientGettingRequest() {
    }
public:

    static ClientState *instance() {
        static ClientGettingRequest cgr;
        return &cgr;
    }

    virtual bool readable(const Client *c) {
        return true;
    }

    virtual bool writable(const Client *c) {
        return false;
    }

    virtual void handle_read(Client *c);
    virtual void handle_close(Client *c);
};


// ------------------ClientRetranslator--------------

class ClientRetranslator : public ClientState {
private:

    ClientRetranslator() {
    }
public:

    ClientState *instance() {
        static ClientRetranslator cr;
        return &cr;
    }

    bool writable(const Client *c) {
    }

    void handle_write(Client *c) {
    }

};
// ------------------ClientCache---------------------

class ClientCache : public ClientState {
private:

    ClientCache() {
    }
public:

    static ClientState *instance() {
        static ClientCache cc;
        return &cc;
    }

    bool readable(const Client *c) {
        return false;
    }

    bool writable(const Client *c) {
        return true; // XXX true!

    }

    void handle_write(Client *c) {
    }

};


// ----------------------Retranslator--------------------------------

class Retranslator {
private:
    Downloader *_d;
    std::list<Client*> _c;
public:
};

// ----------------------ClientState---------------------------------

void ClientState::change_state(Client *c, ClientState *s) {
    c->change_state(s);
}

// -------------------ClientError------------------------------

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

class Retranslator;
// -----------------------Client--------------------------------------

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

// ----------------------ClientGettingRequest-------------------------

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


// --------------Proxy-----------------------------------------------

class Proxy : public AsyncDispatcher {
public:

    Proxy() {
        this->set_reuse_addr(1);
        this->bind(ProxyConfig::listening_port);
        this->listen(ProxyConfig::backlog);
    }

    bool readable() const {
        return true;
    }

    bool writable() const {
        return false;
    }

    void handle_accept() {
        Logger::debug("New client connected");
        new Client(this->accept());
    }
};

// ------------------------Free functions------------------------------

void init_logger() {
    Logger::set_ident(ProxyConfig::ident);
    Logger::set_level(Logger::DEBUG);
}

int main(int argc, char *argv[]) {
    try {
        init_logger();
        new Proxy();
        AsyncDispatcher::loop();
    } catch (std::exception &ex) {
        Logger::error(ex.what());
    }
}
