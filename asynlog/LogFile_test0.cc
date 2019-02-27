#include "LogFile.h"

std::unique_ptr<muduo::LogFile> g_logFile;

int main(int argc, char* argv[])
{
    muduo::string line = "1234567890 abcdefghijklmnopqrstuvwxyz\n";
    char name[256] = { 0 };

    strncpy(name, argv[0], sizeof name - 1);
    g_logFile.reset(new muduo::LogFile(::basename(name), 200*1000));

    for (int i = 0; i < 10000; ++i) {
        g_logFile->append(line.c_str(), line.size());
        usleep(1000);
    }

    return 0;
}
