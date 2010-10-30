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
	
		static void set_indent(const char *ident);
		static void log(enum level priority, const char *fmt, ...);
};

#endif
