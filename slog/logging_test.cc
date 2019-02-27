#include "logging.h"
#include "stringpiece.h"
#include <string>
#include <stdio.h>

int main()
{
    std::string msg("hello world");
    internal::StringPiece sp("Bye Bye");

    SLOG(INFO) << msg;
    SLOG(INFO) << sp;
    SLOG(INFO) << "hello again";
    SLOG(ERROR) << "After this should not happen";

    printf("should never print\n");
    return 0;
}
