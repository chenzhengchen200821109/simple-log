#include "ThreadPool.h"
#include <iostream>

void threadFunc()
{
    std::cout << "I am working" << std::endl;
}

void callback()
{
    std::cout << "--- Thread Pool ---" << std::endl;
}

using namespace muduo;

int main()
{
    static ThreadPool tp("ThreadPool", 10, 3);
    tp.setThreadInitCallback(&callback);
    tp.start();

    for(int i = 0; i < 20; i++) {
        tp.execute(threadFunc);
    }
    tp.stop();

    return 0;
}
