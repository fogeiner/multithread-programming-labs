#include "../libs/Logger/Logger.h"
#include "../libs/TCPSocket/TCPSocket.h"
#include "../libs/Thread/Thread.h"
#include "config.h"

int main(int argc, char *argv[]) {
    Logger::set_level(Logger::DEBUG);

    try {
        TCPSocket l_sock;
        l_sock.set_reuse_addr(1);
        l_sock.bind(ProxyConfig::http_default_port);
        l_sock.listen(ProxyConfig::backlog);

        while (1) {
            TCPSocket *c_sock = l_sock.accept();
        }
    }
}