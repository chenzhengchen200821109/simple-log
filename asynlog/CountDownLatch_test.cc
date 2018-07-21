#include "CountDownLatch.h"
#include <iostream>
#include <thread>

using namespace muduo;

void threadFunc(int i, CountDownLatch* platch)
{
    //platch->countDown();
    std::cout << "Thread: " << i << " finished!" << std::endl;
}

//CountDownLatch latch(10);

int main()
{
    CountDownLatch* platch = new CountDownLatch(10); // 10 work threads plus one main thread
    
    for (int i = 0; i < 10; i++) {
        std::thread t(threadFunc, i, platch);
        t.detach();
    }

    platch->wait(); // main thread waits for all work thread done

    delete platch;
    return 0;
}
