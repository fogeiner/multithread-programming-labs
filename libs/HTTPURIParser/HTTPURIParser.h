#pragma once

#include <string>
#include <ctype.h>
#include <cstdlib>

#ifdef DEBUG
#include <cstdio>
#endif

/*
 *	<scheme>://<netloc>[:<port>]/<path>
 */
class ParsedURI {
public:
    const std::string scheme;
    const std::string port_s;
    const std::string netloc;
    const std::string path;
    const unsigned short port_n;

    ParsedURI(std::string scheme, std::string netloc,
            std::string port_s, std::string path,
            unsigned short port_n) :
    scheme(scheme),
    port_s(port_s),
    netloc(netloc),
    path(path),
    port_n(port_n) {
    }

};

class HTTPURIParser {
private:
    static const std::string http_scheme;
    static const std::string scheme_delimiter;
    static void trim_spaces(std::string &str);
public:
    // if given uri is a valid HTTP URI ParsedURI object is returned;
    // otherwise NULL is returned; returned object must be deleted
    static ParsedURI *parse(std::string uri);
};
