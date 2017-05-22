#include "message_queue_server.h"
#include "marcos.h"
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

message_queue_server::message_queue_server() {
   log = new _log(MQ_LOG_PATH, DEBUG);
   wp = new thread_worker_pool();
   tq = new mutex_task_queue();
}

message_queue_server::~message_queue_server() {
   delete log;
   delete wp;
   delete tq;
}

int message_queue_server::init_mq() { 
   //pthread_create(&poller_pid, NULL, poller, this);
   log->debug("message_queue_server : [init_mq]\n");
   assert((init_poller() != 0));
   wp->start();
   poller(NULL);
   return 0;
}

int message_queue_server::init_poller() {
   log->debug("message_queue_server : [init_poller]\n");
   int ret = 0;
   listenfd = socket(AF_INET, SOCK_STREAM, 0);
   struct sockaddr_in actual_addr;
   actual_addr.sin_family = AF_INET;
   actual_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   actual_addr.sin_port = htons(MQ_PORT);

   // set the listenfd socket to be nonblock.
   fcntl(listenfd, F_SETFL, SOCK_NONBLOCK);

   //set listenfd reused.
   int option = 1;
   setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
   struct sockaddr *addr = (struct sockaddr *)&actual_addr;
   ret = bind(listenfd, addr, sizeof(actual_addr));
   assert(ret != -1);
   listen(listenfd, 50);
   return 0;
}

int message_queue_server::send(std::string msg) {
   return 0;
}

void *message_queue_server::poller(void *arg) {
   epfd = epoll_create1(0); // may change to EPOLL_CLOEXEC.
   assert((epfd != -1)); 

   struct epoll_event event;
   memset(&event, 0, sizeof(event));
   event.events = EPOLLIN | EPOLLET;
   event.data.fd = listenfd;
   epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);
   struct epoll_event events[MAXEVENTS];

   for (;;) {
      int rd_fds = epoll_wait(epfd, events, MAXEVENTS, 1000);
      for (int i = 0; i < rd_fds; ++i) {
         if (events[i].data.fd == listenfd) {
            queue_element *qe = new queue_element();
            qe->_cb = accept_conn;
            qe->arg = this;
            //accept_conn();
         } else {
            if (events[i].events & EPOLLIN) {
               // TODO
               // recv msg and choose subscribers to send.
               queue_element *qe = new queue_element();
               qe->_cb = recv_msg;
               qe->arg = this; 
               tq->push(qe);
               //recv_msg();
            } else {
               if (events[i].events & EPOLLOUT) {
                  // TODO.
                  // we may not use this.
               }
            }
         }
      }
   }
   return NULL;
}

void *message_queue_server::accept_conn(void *arg) {
   message_queue_server *mqs = (message_queue_server *)arg;
   mqs->log->debug("message_queue_server : [accept_conn]\n");

   struct sockaddr_in actual_addr;
   int len = sizeof(struct sockaddr);
   int peer_sock = -1;
   while ((peer_sock = accept(mqs->listenfd, (sockaddr *)&actual_addr, (socklen_t *)&len)) > 0) {
      fcntl(peer_sock, F_SETFL, SOCK_NONBLOCK);
      struct epoll_event event;
      memset(&event, 0, sizeof(event));
      event.events = EPOLLIN | EPOLLET;
      epoll_ctl(mqs->epfd, EPOLL_CTL_ADD, peer_sock, &event); 
   }

   if (peer_sock == -1) {
      if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR)
         perror("accept");
   }
   return NULL;
} 

void *message_queue_server::recv_msg(void *arg) {
   return NULL;
}
