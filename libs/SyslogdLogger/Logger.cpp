#include "Logger.h"

std::string Logger::_ident;
#ifdef MT_LOGGER
Mutex Logger::_m;
#endif
bool Logger::_inited(false);
enum Logger::level Logger::_priority;

void Logger::set_level(enum Logger::level priority){
#ifdef MT_LOGGER
	_m.lock();
#endif
	Logger::_priority = priority;
#ifdef MT_LOGGER
	_m.unlock();
#endif
}

void Logger::set_ident(const char *ident) {
#ifdef MT_LOGGER
	_m.lock();
#endif
	_ident = ident;
#ifdef MT_LOGGER
	_m.unlock();
#endif
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
#ifdef MT_LOGGER
	_m.lock();
#endif
	if(priority <= _priority){
		if(false == _inited){
			openlog(_ident.c_str(), LOG_CONS | LOG_PERROR, LOG_USER);		
			_inited = true;
		}
		vsyslog(priority, fmt, ap);
	}
#ifdef MT_LOGGER
	_m.unlock();
#endif
}


