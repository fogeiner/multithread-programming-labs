#pragma once
#include "../libs/Buffer/VectorBuffer.h"
#include "../libs/TCPSocket/TCPSocket.h"
#include "../libs/HTTPURIParser/HTTPURIParser.h"
#include "../libs/Logger/Logger.h"
#include "BrokenUpHTTPRequest.h"
#include "Cache.h"
#include "CacheEntry.h"
#include <exception>
#include <sstream>

class BadRequestException : public std::exception {
};

class NotImlementedException : public std::exception {
};

class Client {
private:
    TCPSocket *_sock;
    Buffer *_in;
    Buffer *_out;
    BrokenUpHTTPRequest *_request;
    int _bytes_sent;
    CacheEntry *_ce;

public:

    Client(TCPSocket *c_sock);
    ~Client();
    void set_cache_entry(CacheEntry *cache_entry);
    bool parse_request();
    static void *run(void *client_ptr);
};
