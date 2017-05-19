#include "message_queue_server.h"
#include <sys/epoll.h>

int message_queue_server::init_mq() { 
   //pthread_create()
   pthread_create(&poller_pid, NULL, poller, this);
   epfd = epoll_create1(0); // may change to EPOLL_CLOEXEC.
   return 0;
}

int message_queue_server::send(std::string msg) {
   return 0;
}

void *message_queue_server::poller(void *arg) {
   return NULL;
}
