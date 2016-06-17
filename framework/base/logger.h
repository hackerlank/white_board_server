//
//  logger.h
//  gateway
//
//  Created by Fubai Zhong on 10/5/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#ifndef FRAMEWORK_BASE_LOGGER_H_
#define FRAMEWORK_BASE_LOGGER_H_

#include "service.h"
#include "logging.h"

class Logger : public Service {
public:
    Logger();
    ~Logger();
    
public:
    virtual bool Init();
    virtual bool Start();
    virtual bool Stop();
    virtual bool IsRunning();
    
    virtual void Log(LogLevel level, const char* filename, int line, const char* message);
    virtual void Log(const LogMessage & msg);
};

#endif  // FRAMEWORK_BASE_LOGGER_H_
