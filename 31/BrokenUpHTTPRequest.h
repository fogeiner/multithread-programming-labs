#pragma once
#include <string>

struct BrokenUpHTTPRequest {
    const std::string request;
    const std::string method;
    const std::string host;
    const std::string path;
    const unsigned short port;

    BrokenUpHTTPRequest(const std::string request, const std::string method, const std::string host,
            const std::string path, unsigned short int port) :
    request(request),
    method(method),
    host(host),
    path(path),
    port(port) {
    }
};