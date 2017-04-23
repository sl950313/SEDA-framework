#include "tcp_server.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

tcp_server::tcp_server() {
   
}

tcp_server::tcp_server(std::string name) {
   server_name = name;
}

tcp_server::~tcp_server() {
   if (!wp) delete wp;
   if (!tq) delete tq;
   if (!ac) delete ac;
}

void tcp_server::init() {
   tq = new task_queue();
   wp = new thread_worker_pool();
   wp->set_task_queue(tq);
   // TODO : set worker call_back.
   ac = new acceptor(); 
   ac->set_task_queue(tq);
}

void tcp_server::start() {
   init();
   wp->start();
   ac->start();
}

void tcp_server::loop() {
   //ac->loop();
}
