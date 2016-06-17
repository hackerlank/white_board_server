//
//  logging.h
//  gateway
//
//  Created by Fubai Zhong on 10/6/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#ifndef FRAMEWORK_BASE_LOGGING_H_
#define FRAMEWORK_BASE_LOGGING_H_

#include "log4cplus/logger.h"
#include "log4cplus/configurator.h"
//using namespace log4cplus;

#include <stdint.h>
#include <string>

enum LogLevel {
	LOGLEVEL_DEBUG,
    LOGLEVEL_INFO,
    LOGLEVEL_WARNING,
    LOGLEVEL_ERROR,
    LOGLEVEL_FATAL
};

enum logType{
    LOG_TYPE_COMMON,
    LOG_TYPE_SYSLOG,
    LOG_TYPE_CALOG
};

#define LOG(LEVEL)            \
    LogMessage(::LOGLEVEL_##LEVEL, __FILE__, __LINE__, logType::LOG_TYPE_COMMON)
#define SYSLOG(LEVEL)            \
    LogMessage(::LOGLEVEL_##LEVEL, __FILE__, __LINE__, logType::LOG_TYPE_SYSLOG)
#define  CALOG(LEVEL)            \
    LogMessage(::LOGLEVEL_##LEVEL, __FILE__, __LINE__, logType::LOG_TYPE_CALOG)

#define LOG_IF(LEVEL, CONDITION) \
    !(CONDITION) ? (void)0 : LOG(LEVEL)

#define CHECK(EXPRESSION) \
    LOG_IF(FATAL, !(EXPRESSION)) << "CHECK failed: " #EXPRESSION ": "
#define CHECK_EQ(A, B) CHECK((A) == (B))
#define CHECK_NE(A, B) CHECK((A) != (B))
#define CHECK_LT(A, B) CHECK((A) <  (B))
#define CHECK_LE(A, B) CHECK((A) <= (B))
#define CHECK_GT(A, B) CHECK((A) >  (B))
#define CHECK_GE(A, B) CHECK((A) >= (B))

#ifdef NDEBUG

#define DLOG LOG_IF(INFO, false)

#define DCHECK(EXPRESSION) while(false) CHECK(EXPRESSION)
#define DCHECK_EQ(A, B) DCHECK((A) == (B))
#define DCHECK_NE(A, B) DCHECK((A) != (B))
#define DCHECK_LT(A, B) DCHECK((A) <  (B))
#define DCHECK_LE(A, B) DCHECK((A) <= (B))
#define DCHECK_GT(A, B) DCHECK((A) >  (B))
#define DCHECK_GE(A, B) DCHECK((A) >= (B))

#else  // NDEBUG

#define DLOG LOG


#define DCHECK    CHECK
#define DCHECK_EQ CHECK_EQ
#define DCHECK_NE CHECK_NE
#define DCHECK_LT CHECK_LT
#define DCHECK_LE CHECK_LE
#define DCHECK_GT CHECK_GT
#define DCHECK_GE CHECK_GE

#endif  // !NDEBUG

class LogMessage {
public:
    LogMessage(LogLevel level, const char* filename, int line, logType log_type);
    ~LogMessage();

    LogMessage& operator<<(const std::string& value);
    LogMessage& operator<<(const char* value);
    LogMessage& operator<<(char value);
    LogMessage& operator<<(double value);

    LogMessage& operator<<(int8_t value);
    LogMessage& operator<<(int16_t value);
    LogMessage& operator<<(int32_t value);
    LogMessage& operator<<(int64_t value);
    LogMessage& operator<<(uint8_t value);
    LogMessage& operator<<(uint16_t value);
    LogMessage& operator<<(uint32_t value);
    LogMessage& operator<<(uint64_t value);
#ifdef __APPLE__
    LogMessage& operator<<(size_t value);
    LogMessage& operator<<(long value);
#endif
    LogMessage& operator<<(void* value);

private:
    void Finish();

private:
    LogLevel level_;
    const char* filename_;
    int line_;
    std::string message_;
    logType log_type_;

    friend class Logger;
	friend class LogPlusLogger;

	friend class GoogleLogger;
};

class Logger;

Logger* SetDefaultLogger(Logger* logger);







#endif  // FRAMEWORK_BASE_LOGGING_H_
