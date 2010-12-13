#include "ClientGetRequest.h"
#include "../../libs/Logger/Logger.h"
#include "../../libs/HTTPURIParser/HTTPURIParser.h"
#include "Client.h"

#include <string>
#include <sstream>

ClientState *ClientGetRequest::instance() {
    static ClientGetRequest state;
    return &state;
}

bool ClientGetRequest::readable(const Client *c) {
    return true;
}

bool ClientGetRequest::writable(const Client *c) {
    return false;
}

class BadRequestException{};

void ClientGetRequest::handle_read(Client *c) {
    Logger::debug("ClientGetRequest::handle_read()");

    c->recv(c->_in);
    std::string raw_request = *c->_in;

    size_t pos;
    if ((pos = raw_request.find("\r\n\r\n")) == std::string::npos) {
        return;
    }

    Logger::debug("\\r\\n\\r\\n found at the position %ld", pos);

    try {
        std::string request = raw_request;
        std::string method;
        std::string host;
        std::string path;
        unsigned short port;

        std::string word;
        std::istringstream iss(raw_request, std::istringstream::in);

        iss >> word;
        if(word != "GET"){
            throw BadRequestException();
        }
        method = word;

        iss >> word;
        ParsedURI *pu = HTTPURIParser::parse(word);
        if(pu == NULL){
            throw BadRequestException();
        }
        port = pu->port_n == 0 ? 80 : pu->port_n;
        host = pu->netloc;
        path = pu->path;
        delete pu;

        iss >> word;
        if(word != "HTTP/1.1" && word != "HTTP/1.0"){
            throw BadRequestException();
        }

        request.erase(request.end()-2, request.end());
        request.append("Connection: close\r\n\r\n");

        Logger::debug("Client request: %s", ("http://" + host + path).c_str());
    } catch (BadRequestException &ex) {
        Logger::error("ClientGetRequest::handle_read() BadRequestException");
        c->close();
    }

}

void ClientGetRequest::handle_close(Client *c) {
    Logger::debug("ClientGetRequest::handle_close()");
    c->close();
}