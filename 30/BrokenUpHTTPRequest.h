#pragma once
#include <string>

class BrokenUpHTTPRequest {
    std::string _url;
    std::string _request;
    std::string _method;
    std::string _host;
    std::string _path;
    std::string _version;
    unsigned short _port;
public:

    BrokenUpHTTPRequest(const std::string url, const std::string request = "", const std::string method = "", const std::string host = "",
            const std::string path = "", const std::string version = "", unsigned short int port = 0) :
    _url(url),
    _request(request),
    _method(method),
    _host(host),
    _path(path),
    _version(version),
    _port(port) {

    }

    const std::string url() const {
        return _url;
    }

    const std::string request() const {
        return _request;
    }

    const std::string method() const {
        return _method;
    }

    const std::string host() const {
        return _host;
    }

    const std::string path() const {
        return _path;
    }

    const std::string version() const {
        return _version;
    }

    const unsigned short port() const {
        return _port;
    }
};
