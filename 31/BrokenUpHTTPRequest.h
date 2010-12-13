#pragma once
#include <string>

struct BrokenUpHTTPRequest {
    const std::string request;
    const std::string method;
    const std::string host;
    const std::string path;
    const unsigned short port;
    const std::string url;

    BrokenUpHTTPRequest(const std::string request, const std::string method, const std::string host,
            const std::string path, unsigned short int port, const std::string url) :
    request(request),
    method(method),
    host(host),
    path(path),
    port(port),
    url(url)  {

    }
};