#include "tcp_server.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

tcp_server::tcp_server() {
   
}

tcp_server::tcp_server(std::string name) {
   server_name = name;
   log = new _log("log/server.log", DEBUG);
}

tcp_server::~tcp_server() {
   if (!wp) delete wp;
   if (!tq) delete tq;
   if (!ac) delete ac;
   if (!log) delete log;
}

void tcp_server::init() {
   tq = new mutex_task_queue();
   wp = new thread_worker_pool();
   wp->set_task_queue(tq);
   // TODO : set worker call_back.
   ac = new acceptor(log); 
   ac->set_task_queue(tq);
   log->debug("tcp server init success\n");
}

void tcp_server::start() {
   init();
   wp->start();
   log->debug("worker pool running success\n");
   ac->start();
   log->debug("acceptor running success\ntcp server already started\n");
}

void tcp_server::loop() {
   start();
   //ac->loop();
}
