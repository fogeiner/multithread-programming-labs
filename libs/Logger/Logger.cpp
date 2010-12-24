#include "Logger.h"
#include <cstdio>
#include <cstring>

#ifdef MT_LOGGER
Mutex Logger::_m;
#endif
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
	log(WARN, fmt, ap);
	va_end(ap);
}
void Logger::error(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	log(ERROR, fmt, ap);
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
            int fmt_length = strlen(fmt);
            char *cr_fmt = new char[fmt_length + 2];
            for(unsigned int i = 0; i < strlen(fmt); ++i){
                cr_fmt[i] = fmt[i];
            }
            cr_fmt[fmt_length] = '\n';
            cr_fmt[fmt_length + 1] = '\0';
            vfprintf(stderr, cr_fmt, ap);
            delete[] cr_fmt;
	}
#ifdef MT_LOGGER
	_m.unlock();
#endif
}


