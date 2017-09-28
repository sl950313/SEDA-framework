redis-client:
	g++ -o bin/redis-client src/test_redis_client.cpp ../seda_object.cpp ../redis_client.cpp ../log.cpp ../job_queue.cpp -g -lpthread -lhiredis

logger_test: 
	g++ -o bin/logger_test src/test_async_logger.cpp ../log.cpp ../job_queue.cpp -g -lpthread
