//
//  logger.cpp
//  gateway
//
//  Created by Fubai Zhong on 10/5/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#include "logger.h"

#include <inttypes.h>
#include <stdio.h>

Logger::Logger () {
}

Logger::~Logger() {
}

bool Logger::Init() {
    return true;
}

bool Logger::Start() {
    return true;
}

bool Logger::Stop() {
    return true;
}

bool Logger::IsRunning() {
    return true;
}

void Logger::Log(LogLevel level, const char *filename, int line, const char* message) {
    // TODO(fubai): implement log.
    printf("%s", message);
}

void Logger::Log(const LogMessage &msg) {
    Log(msg.level_, msg.filename_, msg.line_, msg.message_.c_str());
}
