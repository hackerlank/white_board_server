//
//  google_logger.cpp
//  game_server_linux
//
//  Created by Fubai Zhong on 11/5/13.
//
//

#include "google_logger.h"

#include <glog/logging.h>

GoogleLogger::GoogleLogger(const char* app_name) : app_name_(app_name) {
    google::InitGoogleLogging(app_name_);

	google::SetLogDestination(google::GLOG_INFO, "glog/server.log");
}

GoogleLogger::~GoogleLogger() {
    
}

bool GoogleLogger::Init() {
    return true;
}

bool GoogleLogger::Start() {
    return true;
}

bool GoogleLogger::Stop() {
    return true;
}

bool GoogleLogger::IsRunning() {
    return true;
}

void GoogleLogger::Log(LogLevel level, const char *filename, int line, const char *message) {
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
    }
}

void GoogleLogger::Log(const LogMessage & msg)
{
	Log(msg.level_, msg.filename_, msg.line_, msg.message_.c_str());
}
