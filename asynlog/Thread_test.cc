#include "Thread.h"
//#include "CurrentThread.h"
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
    //std::cout << "Threads: " << Thread::numCreated() << std::endl;
    //Thread t0(threadFun);
    //Thread t1(threadFun);
    //t0.start();
    //t1.start();
    //std::cout << t0.tid() << std::endl;
    //std::cout << t1.tid() << std::endl;
    //t0.join();
    //t1.join();
    //std::cout << t0.name() << std::endl;
    //t0.join();
    //std::cout << "Main thread" << std::endl;
    return 0;
}
