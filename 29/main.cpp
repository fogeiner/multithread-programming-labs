#include "Proxy/Proxy.h"
#include "../libs/Logger/Logger.h"
#include "config.h"
#include <signal.h>

void init_logger() {
	Logger::set_ident(ProxyConfig::ident);
	Logger::set_level(Logger::EMERG);
}

int main(int argc, char *argv[]) {

	struct sigaction act;

	act.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &act, NULL);

	try {
		init_logger();
		new Proxy();
		AsyncDispatcher::loop();
	} catch (std::exception &ex) {
		Logger::error(ex.what());
	}
}
