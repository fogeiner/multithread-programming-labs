#include "Logger.h"

std::string Logger::_ident;
Mutex Logger::_m;
bool Logger::_inited(false);

void Logger::set_indent(const char *ident) {
	_m.lock();
	_ident = ident;
	_m.unlock();
}

void Logger::log(enum level priority, const char *fmt, ...) {
	_m.lock();

	if(false == _inited){
		openlog(_ident.c_str(), LOG_CONS | LOG_PERROR, LOG_USER);		
		_inited = true;
	}
	
	va_list ap;
	
	va_start(ap, fmt);
	
	// %m is converted into strerror(errno)
	vsyslog(priority, fmt, ap);
	va_end(ap);
			
	_m.unlock();
}


