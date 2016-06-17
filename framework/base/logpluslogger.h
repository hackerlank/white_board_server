
#ifndef FRAMEWORK_BASE_LOGPLUSLOGGER_H_
#define FRAMEWORK_BASE_LOGPLUSLOGGER_H_

#include <log4cplus/logger.h>  
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/loggingmacros.h>


#include "logger.h"

class LogPlusLogger : public Logger
{
public:
	LogPlusLogger() {}
	~LogPlusLogger() {}

	virtual void Log(LogLevel level, const char* filename, int line, const char* message,logType log_type); 
	virtual void Log(const LogMessage & msg);

public:
	static log4cplus::Logger  server_logger_;
	static log4cplus::Logger  sys_server_logger_;
    static log4cplus::Logger ca_server_logger_;
	static log4cplus::Logger  run_logger_;

};







#endif  // FRAMEWORK_BASE_LOGPLUSLOGGER_H_
