logging_test:
	g++ -g -std=c++11 Logging_test.cc Logging.cc LogFile.cc Condition.cc ThreadPool.cc FileUtil.cc LogStream.cc Date.cc Timestamp.cc Thread.cc TimeZone.cc CountDownLatch.cc ProcessInfo.cc -pthread
asynclogging_test:
	g++ -g -std=c++11 AsyncLogging_test.cc AsyncLogging.cc Logging.cc LogFile.cc Condition.cc ThreadPool.cc FileUtil.cc LogStream.cc Date.cc Timestamp.cc Thread.cc TimeZone.cc CountDownLatch.cc ProcessInfo.cc -pthread
thread_test:
	g++ -g -std=c++11 Thread_test.cc Thread.cc Exception.cc CountDownLatch.cc -lpthread
blockqueue_test:
	g++ -g -std=c++11 BoundedBlockingQueue_test.cc Thread.cc Exception.cc CountDownLatch.cc -lpthread
threadpool_test:
	g++ -g -std=c++11 ThreadPool_test.cc ThreadPool.cc Thread.cc CountDownLatch.cc Exception.cc Condition.cc -lpthread
clean:
	rm -rf a.out
