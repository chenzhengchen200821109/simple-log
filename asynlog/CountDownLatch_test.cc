#include "CountDownLatch.h"
#include "Thread.h"
#include <iostream>
#include <pthread.h>

using namespace muduo;

void* threadFunc(void* data)
{
    CountDownLatch* platch = (CountDownLatch*)data;
    platch->countDown();
    std::cout << "Worker thread" << std::endl;
}

//CountDownLatch latch(10);

int main()
{
    CountDownLatch* platch = new CountDownLatch(10); // 10 work threads plus one main thread
   pthread_t tid; 
    for (int i = 0; i < 10; i++) {
        pthread_create(&tid, NULL, &threadFunc, (void*)platch);
    }

    platch->wait(); // main thread waits for all work thread done

    std::cout << "Main thread" << std::endl;
    delete platch;
    return 0;
}
