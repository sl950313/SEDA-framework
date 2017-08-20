LIB=-lpthread -lzmq
OBJ=acceptor.o buffer.o connect.o http_request.o job_queue.o log.o tcp_server.o worker_pool.o http_response.o 
OBJ_MQ=mq_conn.o message_queue_server.o worker_pool.o log.o job_queue.o mq_def.o buffer.o
TARGET=test_nc_server message_queue_server test_mq_client
CC=g++

all: main acceptor-stage#message_queue_server test_mq_client test_nc_server test_stage

main: main.cpp marcos.h IElement.h stage.o config.o stage_handler.o receiver.o stage_control.o worker_pool.o job_queue.o log.o sender.o
	$(CC) -o main main.cpp IElement.h stage_control.o stage.o config.o stage_handler.o receiver.o worker_pool.o job_queue.o log.o sender.o -g $(LIB)

acceptor-stage: acceptor_stage.cpp acceptor.o log.o buffer.o http_request.o connect.o
	$(CC) -o acceptor-stage acceptor_stage.cpp buffer.o http_request.o connect.o log.o acceptor.o worker_pool.o job_queue.o $(LIB) -g

test_stage: test_stage.cpp  stage.o
	$(CC) -o test_stage test_stage.cpp -g $(LIB)

stage.o: stage.h config.h stage.cpp stage_control.o stage_queue.o stage_handler.o worker_pool.o log.o config.o receiver.o
	$(CC) -c stage.cpp -g $(LIB)

receiver.o: receiver.cpp receiver.h log.o
	$(CC) -c receiver.cpp -g $(LIB) 

sender.o: sender.cpp
	$(CC) -c sender.cpp -g $(LIB)

stage_handler.o: IElement.h stage_handler.cpp log.o stage_queue.o worker_pool.o 
	$(CC) -c IElement.h stage_handler.cpp -g -lstdc++

stage_control.o: worker_pool.o log.o
	$(CC) -c stage_control.cpp -g -lstdc++

stage_queue.o: stage_queue.cpp job_queue.o
	$(CC) -c stage_queue.cpp -g -lstdc++

config.o: config.cpp config.h log.o
	$(CC) -c config.cpp -g -lstdc++

test_mq_client: test_mq_client.o message_queue_client.o
	$(CC) -o test_mq_client message_queue_client.o test_mq_client.o -g $(LIB)

message_queue_client.o: message_queue/message_queue_client.h message_queue/message_queue_client.cpp
	$(CC) -c message_queue/message_queue_client.cpp -g

test_mq_client.o: test_mq_client.cpp message_queue/message_queue_client.h
	$(CC) -c test_mq_client.cpp -g

message_queue_server: $(OBJ_MQ)
	$(CC) -o message_queue_server message_queue/mq_server.cpp $(OBJ_MQ) $(LIB) -g

mq_conn.o: message_queue/mq_conn.cpp message_queue/mq_conn.h
	$(CC) -c message_queue/mq_conn.cpp -g

message_queue_server.o: worker_pool.h log.h marcos.h mq_def.h job_queue.h message_queue/message_queue_server.cpp
	$(CC) -c message_queue/message_queue_server.cpp -std=c++11 

mq_def.o: mq_def.h mq_def.cpp
	$(CC) -c mq_def.cpp -g

test_nc_server: $(OBJ)
	$(CC) -o test_nc_server test_server.cpp $(OBJ) $(LIB) -g

acceptor.o: acceptor.cpp acceptor.h job_queue.h struct.h http_request.h http_module.h connect.h
	$(CC) -c acceptor.cpp -g -std=c++11

buffer.o: buffer.cpp buffer.h 
	$(CC) -c buffer.cpp -g

connect.o: connect.cpp connect.h http_request.h http_response.h
	$(CC) -c connect.cpp -g

http_request.o: http_request.cpp http_request.h buffer.h struct.h
	$(CC) -c http_request.cpp -g 

job_queue.o: job_queue.cpp job_queue.h struct.h
	$(CC) -c job_queue.cpp -g $(LIB)

log.o: log.cpp log.h
	$(CC) -c log.cpp -g -lstdc++

tcp_server.o: tcp_server.cpp tcp_server.h log.h connect.h worker_pool.h job_queue.h
	$(CC) -c tcp_server.cpp -g 

worker_pool.o: worker_pool.cpp worker_pool.h connect.h job_queue.h
	$(CC) -c worker_pool.cpp -g $(LIB)

http_response.o: http_response.h http_response.cpp
	$(CC) -c http_response.cpp -g

clean:
	rm -f $(TARGET) test_mq_client.o message_queue_client.o $(OBJ_MQ) $(OBJ) $(TARGET) receiver.o stage.o stage_queue.o stage_handler.o stage_control.o config.o sender.o
