#include "MyThreadPool.h"
#include <iostream>

void threadFunc()
{
    std::cout << "I am working" << std::endl;
}

using namespace muduo;

int main()
{
    static ThreadPool tp("ThreadPool", 2, 5);

    for(int i = 0; i < 10; i++) {
        tp.execute(threadFunc);
    }
    tp.stop();

    return 0;
}
