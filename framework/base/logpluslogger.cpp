#include "logpluslogger.h"

#include <stdio.h>
#include <cinttypes>
#include "logger.h"



//log4cplus ∂¡≈‰÷√Œƒº˛ £°£°
log4cplus::Logger  LogPlusLogger::server_logger_ = log4cplus::Logger::getInstance("server");
log4cplus::Logger LogPlusLogger::sys_server_logger_ = log4cplus::Logger::getInstance("sys_server");
log4cplus::Logger LogPlusLogger::ca_server_logger_ = log4cplus::Logger::getInstance("ca_log");  //CA_LOG
log4cplus::Logger  LogPlusLogger::run_logger_ = log4cplus::Logger::getInstance("run");

void LogPlusLogger::Log(const LogMessage & msg)
{
	Log(msg.level_, msg.filename_, msg.line_, msg.message_.c_str(), msg.log_type_);
}

void LogPlusLogger::Log(LogLevel level, const char* filename, int line, const char* message, logType log_type)
{
	/*
	switch (level) {
	case LOGLEVEL_INFO:
		google::LogMessage(filename, line, google::GLOG_INFO).stream() << message;
		break;
	case LOGLEVEL_WARNING:
		google::LogMessage(filename, line, google::GLOG_WARNING).stream() << message;
		break;
	case LOGLEVEL_ERROR:
		google::LogMessage(filename, line, google::GLOG_ERROR).stream() << message;
		break;
	case LOGLEVEL_FATAL:
		google::LogMessage(filename, line, google::GLOG_FATAL).stream() << message;
		break;
	default:
		LOG(ERROR) << "invalid LogLevel " << (int)(level);
		break;
	}*/

    switch (log_type)
    {
    case LOG_TYPE_COMMON:
        switch (level)
        {
        case LOGLEVEL_DEBUG:
            LOG4CPLUS_DEBUG(LogPlusLogger::server_logger_, message);
            break;
        case LOGLEVEL_INFO:
            LOG4CPLUS_INFO(LogPlusLogger::server_logger_, message);
            break;
        case LOGLEVEL_WARNING:
            LOG4CPLUS_WARN(LogPlusLogger::server_logger_, message);
            break;
        case LOGLEVEL_ERROR:
            LOG4CPLUS_ERROR(LogPlusLogger::server_logger_, message);
            break;
        case LOGLEVEL_FATAL:
            LOG4CPLUS_FATAL(LogPlusLogger::server_logger_, message);
            break;
        default:
            break;
        }
        break;
    case LOG_TYPE_SYSLOG:
        switch (level)
        {
        case LOGLEVEL_DEBUG:
            LOG4CPLUS_DEBUG(LogPlusLogger::sys_server_logger_, message);
            break;
        case LOGLEVEL_INFO:
            LOG4CPLUS_INFO(LogPlusLogger::sys_server_logger_, message);
            break;
        case LOGLEVEL_WARNING:
            LOG4CPLUS_WARN(LogPlusLogger::sys_server_logger_, message);
            break;
        case LOGLEVEL_ERROR:
            LOG4CPLUS_ERROR(LogPlusLogger::sys_server_logger_, message);
            break;
        case LOGLEVEL_FATAL:
            LOG4CPLUS_FATAL(LogPlusLogger::sys_server_logger_, message);
            break;
        default:
            break;
        }
        break;
    case LOG_TYPE_CALOG:
        switch (level)
        {
        case LOGLEVEL_DEBUG:
            LOG4CPLUS_DEBUG(LogPlusLogger::ca_server_logger_, message);
            break;
        case LOGLEVEL_INFO:
            LOG4CPLUS_INFO(LogPlusLogger::ca_server_logger_, message);
            break;
        case LOGLEVEL_WARNING:
            LOG4CPLUS_WARN(LogPlusLogger::ca_server_logger_, message);
            break;
        case LOGLEVEL_ERROR:
            LOG4CPLUS_ERROR(LogPlusLogger::ca_server_logger_, message);
            break;
        case LOGLEVEL_FATAL:
            LOG4CPLUS_FATAL(LogPlusLogger::ca_server_logger_, message);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    
}


