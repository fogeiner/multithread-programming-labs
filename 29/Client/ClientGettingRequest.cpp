#include "ClientGettingRequest.h"
#include "../../libs/Logger/Logger.h"
#include "Client.h"
#include "ClientCache.h"
#include "../Cache/Cache.h"
#include "../config.h"

#define D
//#undef D

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

void ClientGettingRequest::handle_read(Client *c) {
    try {
        Logger::debug("ClientGettingRequest::handle_read()");

        c->recv(c->_b);

        // checking if there's \r\n\r\n in request so far
        const char *b = c->_b->buf();
        const char *p;
        p = strstr(b, "\r\n\r\n");

        // there's \r\n\r\n in the buffer; time to parse!
        if (p != NULL) {
            Logger::debug("http_delimiter found; parsing request");


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

            // we have to support only HEAD and GET
            // (GET|HEAD) URL HTTP/1.X
            p = strstr(query, "GET");
            if (p == NULL || p != query) {
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
                ce = new CacheEntry(std::string(url), c->_b);
                c->_ce = ce;
                ce->add_client(c);
                cache->add(std::string(url), ce);
                c->change_state(ClientCache::instance());
                ce->activate();

            } else {

                Logger::debug("Cache entry for URL found");
                c->_ce = ce;
                ce->add_client(c);
                c->change_state(ClientCache::instance());
            }
        }
    } catch (ClientBadRequestException &ex) {
        Logger::debug("Bad request found");
        c->error(ProxyConfig::bad_request_msg);
    } catch (RecvException &ex) {
        Logger::error(ex.what());
        c->close();
    }
}

void ClientGettingRequest::handle_close(Client *c) {
    c->close();
}

