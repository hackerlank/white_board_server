//
//  logging.cpp
//  gateway
//
//  Created by Fubai Zhong on 10/6/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#include "logging.h"

#include <stdio.h>
#include <cinttypes>

#include "logger.h"



class PrintfLogger;

class PrintfLogger : public Logger {
public:
    PrintfLogger() {}
    ~PrintfLogger() {}

    virtual void Log(LogLevel level, const char* filename, int line, const char* message) {

        printf("[PrintfLogger] %s", message);
    }
};



static PrintfLogger printf_logger;
static Logger* logger_ = &printf_logger;

Logger* SetDefaultLogger(Logger* new_logger) {
    Logger* old = logger_;
    logger_ = new_logger;
    return old;
}

LogMessage::LogMessage(LogLevel level, const char *filename, int line, logType log_type)
    : level_(level)
    , filename_(filename)
    , line_(line) 
    , log_type_(log_type){
}

LogMessage::~LogMessage() {
    Finish();
}

LogMessage& LogMessage::operator<<(const std::string& value) {
    message_ += value;
    return *this;
}

LogMessage& LogMessage::operator<<(const char* value) {
	if (value == nullptr)
		return *this;
    message_ += value;
    return *this;
}

// Since this is just for logging, we don't care if the current locale changes
// the results -- in fact, we probably prefer that.  So we use snprintf()
// instead of Simple*toa().
#undef DECLARE_STREAM_OPERATOR
#define DECLARE_STREAM_OPERATOR(TYPE, FORMAT)                       \
    LogMessage& LogMessage::operator<<(TYPE value) {                  \
    /* 128 bytes should be big enough for any of the primitive */   \
    /* values which we print with this, but well use snprintf() */  \
    /* anyway to be extra safe. */                                  \
    char buffer[128];                                               \
    snprintf(buffer, sizeof(buffer), FORMAT, value);                \
    /* Guard against broken MSVC snprintf(). */                     \
    buffer[sizeof(buffer)-1] = '\0';                                \
    message_ += buffer;                                             \
    return *this;                                                   \
}


DECLARE_STREAM_OPERATOR(char            , "%c"  )
DECLARE_STREAM_OPERATOR(double          , "%g"  )

DECLARE_STREAM_OPERATOR(int8_t          , "%" PRIi8)
DECLARE_STREAM_OPERATOR(int16_t         , "%" PRIi16)
DECLARE_STREAM_OPERATOR(int32_t         , "%" PRIi32)
DECLARE_STREAM_OPERATOR(int64_t         , "%" PRIi64)
DECLARE_STREAM_OPERATOR(uint8_t         , "%" PRIu8)
DECLARE_STREAM_OPERATOR(uint16_t        , "%" PRIu16)
DECLARE_STREAM_OPERATOR(uint32_t        , "%" PRIu32)
DECLARE_STREAM_OPERATOR(uint64_t        , "%" PRIu64)

#ifdef __APPLE__
DECLARE_STREAM_OPERATOR(size_t          , "%zu")
DECLARE_STREAM_OPERATOR(long            , "%ld"  )
#endif

DECLARE_STREAM_OPERATOR(void*           , "%p")

#undef DECLARE_STREAM_OPERATOR

void LogMessage::Finish() {
    if ((*message_.crbegin()) != '\n') {
        message_.append("\n");
    }

    if (logger_) {
        logger_->Log(*this);
    }
}
