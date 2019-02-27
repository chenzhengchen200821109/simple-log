#include "logging.h"
#include "stringpiece.h"
#include <stdio.h>
#include <stdlib.h>

#define MIN_LOG_LEVEL LOGLEVEL_INFO
#define MAX_LOG_LEVEL LOGLEVEL_FATAL

namespace internal 
{

    void DefaultLogHandler(LogLevel level, const char* filename, int line, const string& message) 
    {
        if (level < MIN_LOG_LEVEL || level > MAX_LOG_LEVEL) {
            fprintf(stderr, "[slog only suport 4 levels: INFO WARNING ERROR FATAL\n");
            exit(-1);
        }
        static const char* level_names[] = { "INFO", "WARNING", "ERROR", "FATAL" };

        // We use fprintf() instead of cerr because we want this to work at static
        // initialization time.
        fprintf(stderr, "[slog: %s: %s: %d] %s\n", level_names[level], filename, line, message.c_str());
        fflush(stderr);  // just in case
        // terminate program when occurred errors or fatals
        if (level == LOGLEVEL_ERROR || level == LOGLEVEL_FATAL)
            exit(-1);
    }

    void NullLogHandler(LogLevel level, const char* filename, int line, const string& message) 
    {
        // Nothing.
    }

    static LogHandler* log_handler_ = &DefaultLogHandler;

    LogMessage& LogMessage::operator<<(const string& value) 
    {
        message_ += value;
        return *this;
    }

    LogMessage& LogMessage::operator<<(const char* value) 
    {
        message_ += value;
        return *this;
    }

    LogMessage& LogMessage::operator<<(const StringPiece& value) 
    {
        message_ += value.toString();
        return *this;
    }

    // Since this is just for logging, we don't care if the current locale changes
    // the results -- in fact, we probably prefer that.  So we use snprintf()
    // instead of Simple*toa().
    #undef DECLARE_STREAM_OPERATOR
    #define DECLARE_STREAM_OPERATOR(TYPE, FORMAT)                       \
        LogMessage& LogMessage::operator<<(TYPE value) {                \
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

    DECLARE_STREAM_OPERATOR(char         , "%c" )
    DECLARE_STREAM_OPERATOR(int          , "%d" )
    DECLARE_STREAM_OPERATOR(unsigned int , "%u" )
    DECLARE_STREAM_OPERATOR(long         , "%ld")
    DECLARE_STREAM_OPERATOR(unsigned long, "%lu")
    DECLARE_STREAM_OPERATOR(double       , "%g" )
    DECLARE_STREAM_OPERATOR(void*        , "%p" )
    DECLARE_STREAM_OPERATOR(long long    , "%lld")
    DECLARE_STREAM_OPERATOR(unsigned long long, "%llu")
    #undef DECLARE_STREAM_OPERATOR

    LogMessage::LogMessage(LogLevel level, const char* filename, int line) : level_(level), filename_(filename), line_(line) {}
    LogMessage::~LogMessage() {}

    void LogMessage::Finish() 
    {
        log_handler_(level_, filename_, line_, message_);
        if (level_ == LOGLEVEL_FATAL) {
            abort();
        }
    }

    void LogFinisher::operator=(LogMessage& other) 
    {
        other.Finish();
    }

}  // namespace internal

LogHandler* SetLogHandler(LogHandler* new_func) 
{
    LogHandler* old = internal::log_handler_;
    if (old == &internal::NullLogHandler) {
        old = NULL;
    }
    if (new_func == NULL) {
        internal::log_handler_ = &internal::NullLogHandler;
    } else {
        internal::log_handler_ = new_func;
    }
    return old;
}

