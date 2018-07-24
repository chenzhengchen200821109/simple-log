#include "BoundedBlockingQueue.h"
#include <iostream>

using namespace muduo;

void* producer(void* arg) 
{
    static_cast<BoundedBlockingQueue<int> *>(arg)->put(10);
}

void* consumer(void* arg)
{
    static_cast<BoundedBlockingQueue<int> *>(arg)->take();
}

int main()
{
    int i;
    pthread_t tid[40];
    BoundedBlockingQueue<int>* bq = new BoundedBlockingQueue<int>(10);
    
    // create producers
    for (i = 0; i < 20; i++) {
        pthread_create(&tid[i], NULL, &producer, (void *)bq);
    }

    // create consumers
    for ( ; i < 40; i++) {
        pthread_create(&tid[i], NULL, &consumer, (void *)bq);
    }

    for (i = 0; i < 40; i++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
