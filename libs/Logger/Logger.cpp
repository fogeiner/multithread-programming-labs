#include "Logger.h"

std::string Logger::_ident;
Mutex Logger::_m;
bool Logger::_inited(false);

void Logger::set_ident(const char *ident) {
	_m.lock();
	_ident = ident;
	_m.unlock();
}

void Logger::debug(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	log(DEBUG, fmt, ap);
	va_end(ap);
}
void Logger::info(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	log(INFO, fmt, ap);
	va_end(ap);
}

void Logger::warning(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	log(WARNING, fmt, ap);
	va_end(ap);
}
void Logger::error(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	log(ERR, fmt, ap);
	va_end(ap);
}
void Logger::critical(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	log(CRIT, fmt,  ap);
	va_end(ap);
}

void Logger::emergent(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	log(EMERG, fmt,  ap);
	va_end(ap);
}

void Logger::alert(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	log(ALERT, fmt,  ap);
	va_end(ap);
}

void Logger::notice(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	log(NOTICE, fmt,  ap);
	va_end(ap);
}
void Logger::log(enum level priority, const char *fmt, va_list ap){
	_m.lock();
	if(false == _inited){
		openlog(_ident.c_str(), LOG_CONS | LOG_PERROR, LOG_USER);		
		_inited = true;
	}
	vsyslog(priority, fmt, ap);
	_m.unlock();
}


