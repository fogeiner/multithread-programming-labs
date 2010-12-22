#include "../libs/Logger/Logger.h"
#include "../libs/TCPSocket/TCPSocket.h"
#include "../libs/Thread/Thread.h"

#include "Cache.h"
#include "Client.h"
#include "config.h"

int main(int argc, char *argv[]) {
    Logger::set_level(Logger::INFO);
    Cache::init();

    try {
        TCPSocket l_sock;
        l_sock.set_reuse_addr(1);
        l_sock.bind(ProxyConfig::listening_port);
        l_sock.listen(ProxyConfig::backlog);

        Logger::info("Proxy is bound to %d port", ProxyConfig::listening_port);

        while (1) {
            // accepting new client
            TCPSocket *c_sock = l_sock.accept();
            Client *client = new Client(c_sock);
            Thread client_thread(Client::run, client);  
            client_thread.run();
            client_thread.detach();
        }
    } catch (AcceptException &ex) {
        Logger::error("main() Accept: %s", ex.what());
    } catch (ThreadException &ex) {
        Logger::error("main() Thread: %s", ex.what());
    } catch (BindException &ex) {
        Logger::error("main() Bind: %s", ex.what());
    } catch (std::exception &ex) {
        Logger::error("main() Other: %s", ex.what());
    }
}
