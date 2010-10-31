#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>
#include <stdarg.h>
#include <string>
#include <assert.h>
#include "../Mutex/Mutex.h"

class Logger {
	private:
		static std::string _ident;
		static Mutex _m;
		static bool _inited;

		enum level {EMERG = LOG_EMERG, ALERT = LOG_ALERT, 
			CRIT = LOG_CRIT, ERR = LOG_ERR, 
			WARNING = LOG_WARNING, NOTICE = LOG_NOTICE, 
			INFO = LOG_INFO, DEBUG = LOG_DEBUG};

		static void log(enum level priority, const char *fmt, va_list ap);
	
		Logger(){
			assert(false);
		}
	
		~Logger(){
			assert(false);
		}
	
	public:
	
		static void set_ident(const char *ident);
		static void debug(const char *fmt, ...);
		static void info(const char *fmt, ...);
		static void warning(const char *fmt, ...);
		static void error(const char *fmt, ...);
		static void critical(const char *fmt, ...);
		static void emergent(const char *fmt, ...);
		static void alert(const char *fmt, ...);
		static void notice(const char *fmt, ...);
};

#endif
