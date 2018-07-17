#include "logging.h"
#include "stringpiece.h"
#include <string>

int main()
{
    std::string msg("hello world");
    internal::StringPiece sp("Bye Bye");

    SLOG(INFO) << msg;
    SLOG(INFO) << sp;

    return 0;
}
