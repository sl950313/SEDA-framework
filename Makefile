
LIB=-lpthread

all:
	g++ -o test_worker_pool test_worker_pool.cpp worker_pool.cpp job_queue.cpp -g $(LIB)
