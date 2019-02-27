#ifndef LOGGING_H
#define LOGGING_H

#include <string>

enum LogLevel {
  LOGLEVEL_INFO = 0,     // Informational.  This is never actually used by libprotobuf.
  LOGLEVEL_WARNING,      // Warns about issues that, although not technically a
                         // problem now, could cause problems in the future.  For
                         // example, a warning will be printed when parsing a
                         // message that is near the message size limit.
  LOGLEVEL_ERROR,        // An error occurred which should never happen during
                         // normal use.
  LOGLEVEL_FATAL,        // An error occurred from which the library cannot
                         // recover.  This usually indicates a programming error
                         // in the code which calls the library, especially when
                         // compiled in debug mode.
};

//class StringPiece;

namespace internal {

    class StringPiece;
    class LogFinisher;

    class LogMessage 
    {
        public:
            LogMessage(LogLevel level, const char* filename, int line);
            ~LogMessage();

            LogMessage& operator<<(const std::string& value);
            LogMessage& operator<<(const char* value);
            LogMessage& operator<<(char value);
            LogMessage& operator<<(int value);
            LogMessage& operator<<(unsigned int value);
            LogMessage& operator<<(long value);
            LogMessage& operator<<(unsigned long value);
            LogMessage& operator<<(long long value);
            LogMessage& operator<<(unsigned long long value);
            LogMessage& operator<<(double value);
            LogMessage& operator<<(void* value);
            LogMessage& operator<<(const StringPiece& value);
        
        private:
            friend class LogFinisher;
            void Finish();

            LogLevel level_;
            const char* filename_;
            int line_;
            std::string message_;
    };

    // Used to make the entire "LOG(BLAH) << etc." expression have a void return
    // type and print a newline after each message.
    class LogFinisher 
    {
        public:
            void operator=(LogMessage& other);
    };

}  // namespace internal

#define SLOG(LEVEL)                             \
  internal::LogFinisher() =                           \
    internal::LogMessage(                             \
      LOGLEVEL_##LEVEL, __FILE__, __LINE__)
#define SLOG_IF(LEVEL, CONDITION) \
  !(CONDITION) ? (void)0 : SLOG(LEVEL)

#define SLOG_CHECK(EXPRESSION) \
  SLOG_IF(FATAL, !(EXPRESSION)) << "CHECK failed: " #EXPRESSION ": "
#define SLOG_CHECK_EQ(A, B) SLOG_CHECK((A) == (B))
#define SLOG_CHECK_NE(A, B) SLOG_CHECK((A) != (B))
#define SLOG_CHECK_LT(A, B) SLOG_CHECK((A) <  (B))
#define SLOG_CHECK_LE(A, B) SLOG_CHECK((A) <= (B))
#define SLOG_CHECK_GT(A, B) SLOG_CHECK((A) >  (B))
#define SLOG_CHECK_GE(A, B) SLOG_CHECK((A) >= (B))

namespace internal 
{
    template<typename T>
    T* CheckNotNull(const char* /* file */, int /* line */, const char* name, T* val) {
        if (val == NULL) {
            SLOG(FATAL) << name;
        }
    return val;
    }
}  // namespace internal

#define SLOG_CHECK_NOTNULL(A) \
    internal::CheckNotNull(     \
      __FILE__, __LINE__, "'" #A "' must not be NULL", (A))

typedef void LogHandler(LogLevel level, const char* filename, int line,
                        const std::string& message);

// The protobuf library sometimes writes warning and error messages to
// stderr.  These messages are primarily useful for developers, but may
// also help end users figure out a problem.  If you would prefer that
// these messages be sent somewhere other than stderr, call SetLogHandler()
// to set your own handler.  This returns the old handler.  Set the handler
// to NULL to ignore log messages (but see also LogSilencer, below).
//
// Obviously, SetLogHandler is not thread-safe.  You should only call it
// at initialization time, and probably not from library code.  If you
// simply want to suppress log messages temporarily (e.g. because you
// have some code that tends to trigger them frequently and you know
// the warnings are not important to you), use the LogSilencer class
// below.
LogHandler* SetLogHandler(LogHandler* new_func);

// Create a LogSilencer if you want to temporarily suppress all log
// messages.  As long as any LogSilencer objects exist, non-fatal
// log messages will be discarded (the current LogHandler will *not*
// be called).  Constructing a LogSilencer is thread-safe.  You may
// accidentally suppress log messages occurring in another thread, but
// since messages are generally for debugging purposes only, this isn't
// a big deal.  If you want to intercept log messages, use SetLogHandler().
//namespace internal 
//{
//    class LogSilencer 
//    {
//        public:
//            LogSilencer();
//            ~LogSilencer();
//    };
//}

#endif  // LOGGING_H
