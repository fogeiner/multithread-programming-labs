#pragma once
#include <string>

struct BrokenUpHTTPRequest {
    const std::string url;
    const std::string request;
    const std::string method;
    const std::string host;
    const std::string path;
    const unsigned short port;

    BrokenUpHTTPRequest(const std::string url, const std::string request = "", const std::string method = "", const std::string host = "",
            const std::string path = "", unsigned short int port = 0) :
    url(url),
    request(request),
    method(method),
    host(host),
    path(path),
    port(port) {

    }
};