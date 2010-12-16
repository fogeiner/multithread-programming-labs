#pragma once
#include <string>

struct BrokenUpHTTPRequest {
    const std::string url;
    const std::string request;
    const std::string method;
    const std::string host;
    const std::string path;
    const std::string version;
    const unsigned short port;

    BrokenUpHTTPRequest(const std::string url, const std::string request = "", const std::string method = "", const std::string host = "",
            const std::string path = "", const std::string version = "", unsigned short int port = 0) :
    url(url),
    request(request),
    method(method),
    host(host),
    path(path),
    version(version),
    port(port) {

    }
};
