LIB=-lpthread
OBJ=acceptor.o buffer.o connect.o http_request.o job_queue.o log.o tcp_server.o worker_pool.o http_response.o
TARGET=test_nc_server

all: test_nc_server

test_nc_server: $(OBJ)
	g++ -o test_nc_server test_server.cpp $(OBJ) $(LIB) -g

acceptor.o: acceptor.cpp acceptor.h job_queue.h struct.h http_request.h http_module.h connect.h
	g++ -c acceptor.cpp -g -std=c++11

buffer.o: buffer.cpp buffer.h 
	g++ -c buffer.cpp -g

connect.o: connect.cpp connect.h http_request.h http_response.h
	g++ -c connect.cpp -g

http_request.o: http_request.cpp http_request.h buffer.h struct.h
	g++ -c http_request.cpp -g 

job_queue.o: job_queue.cpp job_queue.h struct.h
	g++ -c job_queue.cpp -g $(LIB)

log.o: log.cpp log.h
	g++ -c log.cpp -g

tcp_server.o: tcp_server.cpp tcp_server.h log.h connect.h worker_pool.h job_queue.h
	g++ -c tcp_server.cpp -g 

worker_pool.o: worker_pool.cpp worker_pool.h connect.h job_queue.h
	g++ -c worker_pool.cpp -g $(LIB)

http_response.o: http_response.h http_response.cpp
	g++ -c http_response.cpp -g

clean:
	rm $(OBJ) $(TARGET)
