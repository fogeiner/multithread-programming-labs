#include <exception>
#include <cstring>
#include <alloca.h>

#include <list>
#include <map>
#include <cassert>

#include "Proxy/Proxy.h"
#include "../libs/Logger/Logger.h"
#include "config.h"
#define D

#ifdef D
#include <cstdio>
#endif



// --------------Proxy-----------------------------------------------



// ------------------------Free functions------------------------------

void init_logger() {
    Logger::set_ident(ProxyConfig::ident);
    Logger::set_level(Logger::DEBUG);
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
