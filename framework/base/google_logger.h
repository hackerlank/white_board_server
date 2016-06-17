//
//  google_logger.h
//  game_server_linux
//
//  Created by Fubai Zhong on 11/5/13.
//
//

#ifndef FRAMEWORK_SERVER_GOOGLE_LOGGER_H_
#define FRAMEWORK_SERVER_GOOGLE_LOGGER_H_

#include "logger.h"

class GoogleLogger : public Logger {
public:
    GoogleLogger(const char* app_name);
    ~GoogleLogger();
    
public:
    virtual bool Init();
    virtual bool Start();
    virtual bool Stop();
    virtual bool IsRunning();
    
    virtual void Log(LogLevel level, const char* filename, int line, const char* message);

	virtual void Log(const LogMessage & msg);
    
private:
    const char* app_name_;
};

#endif  // FRAMEWORK_SERVER_GOOGLE_LOGGER_H_
