#pragma once 

#include <syslog.h>
#include <stdarg.h>
#include <string>
#include <assert.h>

#ifdef MT_LOGGER
#include "../Mutex/Mutex.h"
#endif

class Logger {
	Logger(){
		assert(false);
	}

	~Logger(){
		assert(false);
	}

	public:
	enum level {EMERG = LOG_EMERG, ALERT = LOG_ALERT, 
		CRIT = LOG_CRIT, ERR = LOG_ERR, 
		WARNING = LOG_WARNING, NOTICE = LOG_NOTICE, 
		INFO = LOG_INFO, DEBUG = LOG_DEBUG};

	static void set_level(enum Logger::level priority);
	static void set_ident(const char *ident);
	static void debug(const char *fmt, ...);
	static void info(const char *fmt, ...);
	static void warning(const char *fmt, ...);
	static void error(const char *fmt, ...);
	static void critical(const char *fmt, ...);
	static void emergent(const char *fmt, ...);
	static void alert(const char *fmt, ...);
	static void notice(const char *fmt, ...);

	private:
#ifdef MT_LOGGER
	static Mutex _m;
#endif 

	static std::string _ident;
	static bool _inited;
	static enum Logger::level _priority;

	static void log(enum Logger::level priority, const char *fmt, va_list ap);

};
