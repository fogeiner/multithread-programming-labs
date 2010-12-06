#include "Proxy/Proxy.h"
#include "../libs/Logger/Logger.h"
#include "config.h"

void init_logger() {
    Logger::set_ident(ProxyConfig::ident);
    Logger::set_level(Logger::EMERG);
}

int main(int argc, char *argv[]) {
    try {
        init_logger();
        new Proxy();
        AsyncDispatcher::loop();
    } catch (std::exception &ex) {
        Logger::error(ex.what());
    }
}
