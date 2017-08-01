LIB=-lpthread -lstdc++ -lzmq
OBJ=acceptor.o buffer.o connect.o http_request.o job_queue.o log.o tcp_server.o worker_pool.o http_response.o 
OBJ_MQ=mq_conn.o message_queue_server.o worker_pool.o log.o job_queue.o mq_def.o buffer.o
TARGET=test_nc_server message_queue_server test_mq_client

all: main #message_queue_server test_mq_client test_nc_server test_stage

main: main.cpp IElement.h stage.o config.o stage_handler.o receiver.o stage_control.o worker_pool.o job_queue.o log.o
	g++ -o main main.cpp IElement.h stage_control.o stage.o config.o stage_handler.o receiver.o worker_pool.o job_queue.o log.o -g $(LIB)

test_stage: test_stage.cpp  stage.o
	g++ -o test_stage test_stage.cpp -g $(LIB)

stage.o: stage.h config.h stage.cpp stage_control.o stage_queue.o stage_handler.o worker_pool.o log.o config.o receiver.o
	g++ -c stage.cpp -g $(LIB)

receiver.o: receiver.cpp receiver.h log.o
	g++ -c receiver.cpp -g $(LIB) 

stage_handler.o: IElement.h stage_handler.cpp log.o stage_queue.o worker_pool.o 
	g++ -c IElement.h stage_handler.cpp -g -lstdc++

stage_control.o: worker_pool.o log.o
	g++ -c stage_control.cpp -g -lstdc++

stage_queue.o: stage_queue.cpp job_queue.o
	g++ -c stage_queue.cpp -g -lstdc++

#event_core.o: event_core.cpp event_core.h socket.o config.o loop.o
	#g++ -c event_core.cpp -g

config.o: config.cpp config.h log.o
	g++ -c config.cpp -g -lstdc++

#socket.o: socket.cpp socket.h log.o
	#g++ -c socket.cpp log.o -g

#loop.o: loop.cpp loop.h stage_queue.o socket.o
	#g++ -c loop.cpp stage_queue.o socket.o -g

test_mq_client: test_mq_client.o message_queue_client.o
	g++ -o test_mq_client message_queue_client.o test_mq_client.o -g $(LIB)

message_queue_client.o: message_queue/message_queue_client.h message_queue/message_queue_client.cpp
	g++ -c message_queue/message_queue_client.cpp -g

test_mq_client.o: test_mq_client.cpp message_queue/message_queue_client.h
	g++ -c test_mq_client.cpp -g

message_queue_server: $(OBJ_MQ)
	g++ -o message_queue_server message_queue/mq_server.cpp $(OBJ_MQ) $(LIB) -g

mq_conn.o: message_queue/mq_conn.cpp message_queue/mq_conn.h
	g++ -c message_queue/mq_conn.cpp -g

message_queue_server.o: worker_pool.h log.h marcos.h mq_def.h job_queue.h message_queue/message_queue_server.cpp
	g++ -c message_queue/message_queue_server.cpp -std=c++11 

mq_def.o: mq_def.h mq_def.cpp
	g++ -c mq_def.cpp -g

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
	g++ -c log.cpp -g -lstdc++

tcp_server.o: tcp_server.cpp tcp_server.h log.h connect.h worker_pool.h job_queue.h
	g++ -c tcp_server.cpp -g 

worker_pool.o: worker_pool.cpp worker_pool.h connect.h job_queue.h
	g++ -c worker_pool.cpp -g $(LIB)

http_response.o: http_response.h http_response.cpp
	g++ -c http_response.cpp -g

clean:
	rm -f $(TARGET) test_mq_client.o message_queue_client.o $(OBJ_MQ) $(OBJ) $(TARGET) receiver.o stage.o stage_queue.o stage_handler.o stage_control.o config.o
