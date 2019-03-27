#include "Thread.h"
#include <iostream>

void threadFun()
{
    std::cout << "Worker thread" << std::endl;
}

using namespace muduo;

int main()
{
    for (int i = 0; i < 10; i++) {
        Thread t(threadFun);
        std::cout << "started: " << t.started() << std::endl;
        t.start();
        t.join();
        std::cout << t.tid() << std::endl;
        std::cout << t.name() << std::endl;
        std::cout << t.tidToString() << std::endl;
    } 
    return 0;
}
