# slog
implement a simple logger based on google's protobuf logging facility  

## usage  
only support four logging levels: INFO WARNING ERROR FATAL.  
programs will be terminated when logging level is ERROR or FATAL. 

### basic usage:
    SLOG(INFO) << "logging message";  
    SLOG(WARNING) << "logging message";  
    SLOG(ERROR) << "logging message";
    SLOG(FATAL) << "logging message";

### slog check usage:
    SLOG_CHECK_EQ(expression);
    SLOG_CHECK_NE(expression);
    SLOG_CHECK_LT(expression);
    SLOG_CHECK_LE(expression);
    SLOG_CHECK_GT(expression);
    SLOG_CHECK_GE(expression);
