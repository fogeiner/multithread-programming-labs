#include "../libs/Logger/Logger.h"
#include "../libs/AsyncDispatcher/AsyncDispatcher.h"
#include "../libs/Buffer/VectorBuffer.h"

#include <exception>
#include <cstring>
#include <alloca.h>

#include "config.h"

#define D

#ifdef D
#include <cstdio>
#endif



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

protected:
    void change_state(Downloader *c, DownloaderState *s);
};

// ------------------Downloader---------------------------
class Downloader : public AsyncDispatcher {
    friend class DownloaderState;

    void change_state(DownloaderState* s) {
        this->_state = s;
    }
    DownloaderState *_state;
    Buffer *_b;

public:

    Downloader(TCPSocket *sock) : AsyncDispatcher(sock) {
        this->_state = DownloaderConnecting::instance();
        this->_b = new VectorBuffer();
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

    virtual void handle_read(Client *c) {
    }
    virtual void handle_write(Client *c);

    virtual void handle_close(Client *c) {
    }
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

    virtual void handle_write(Client *c) {
    }
    virtual void handle_close(Client *c);

};

// -----------------------Client--------------------------------------
class Client : public AsyncDispatcher {
    friend class ClientState;
    friend class ClientGettingRequest;
    friend class ClientError;

    void change_state(ClientState* s) {
        this->_state = s;
    }
    ClientState *_state;
    Buffer *_b;

public:

    Client(TCPSocket *sock) : AsyncDispatcher(sock) {
        this->_state = ClientGettingRequest::instance();
        this->_b = new VectorBuffer();
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

// ----------------------ClientState---------------------------------

void ClientState::change_state(Client *c, ClientState *s) {
    c->change_state(s);
}

// -------------------ClientBadRequest------------------------------

bool ClientError::writable(const Client *c) {
    return c->_b->size() > 0;
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

                bool is_get = true;
                // we have to support only HEAD and GET
                // (GET|HEAD) URL HTTP/1.X
                p = strstr(query, "GET");
                if (p == NULL || p != query) {
                    p = strstr(query, "HEAD");
                    is_get = false;
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
            } catch (ClientBadRequestException &ex) {
                Logger::debug("Bad request found");
                c->_b->clear();
                ClientState *s = ClientError::instance();
                c->change_state(s);
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
