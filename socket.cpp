#include "socket.h"
#include "log.h" 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h> 
#include <unistd.h>

/*
 * 完成所有的客户端socket初始化动作
 */
Socket::Socket(std::string ip, int port) { 
   connect(ip, port);
}

/*
 * 完成所有的服务端socket初始化动作
 */
Socket::Socket(int listen_port) {
   this->listen_port = listen_port;
   bool ret = initServer();
   if (ret) {
      LogUtil::debug("Socket [initServer] error");
      return ;
   }
}

bool Socket::initServer() {
   int ret = 0;
   if ((ret = (listenfd = socket(AF_INET, SOCK_STREAM, 0))) == -1) {
      // log here.
      LogUtil::debug("Socket : [initServer] create socket error");
      //fprintf(stderr, "create socket error\n");
      return false;
   }
   struct sockaddr_in actual_addr;
   actual_addr.sin_family = AF_INET;
   actual_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   actual_addr.sin_port = htons(listen_port);

   // set the listenfd socket to be nonblock.
   fcntl(listenfd, F_SETFL, SOCK_NONBLOCK);

   //set listenfd reused.
   int option = 1;
   setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

   struct sockaddr *addr = (struct sockaddr *)&actual_addr;
   //addr.sa_family.
   if ((ret = bind(listenfd, addr, sizeof(actual_addr))) == -1) {
      fprintf(stderr, "bind socket error : %s\n", strerror(errno));
      return false;
   } 
   listen(listenfd, 50);
   return true;
}

/*
void Socket::loop() { 
   epfd = epoll_create1(EPOLL_CLOEXEC);
   struct epoll_event event;
   memset(&event, 0, sizeof(event));
   event.events = EPOLLIN | EPOLLET;
   event.data.fd = listenfd;
   epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);
   struct epoll_event events[MAXEVENTS];
   //log->info("acceptor looping\n");
   LogUtil::debug("Socket : [loop] running");

   for (;;) {
      int rd_fds = epoll_wait(epfd, events, MAXEVENTS, 1000);
      for (int i = 0; i < rd_fds; ++i) {
         if (events[i].data.fd == listenfd) {
            //acceptor *ac = this;
            accept_conn();
         } else {
            // TODO.
            if (events[i].events & EPOLLIN) {
               connection *conn = (connection *)events[i].data.ptr;
               conn->cb = read_conn;
               queue_element *qe = new queue_element();
               qe->_cb = conn->cb;
               qe->arg = (void *)conn;
               tq->push((void *)qe);
               log->debug("acceptor : [epoll_loop]. tq->push read_conn\n");
            } else {
               if (events[i].events & EPOLLOUT) {
                  connection *conn = (connection *)events[i].data.ptr;
                  conn->cb = write_conn;
                  queue_element *qe = new queue_element();
                  qe->_cb = write_conn;
                  qe->arg = (void *)conn;
                  tq->push((void *)qe); 
                  log->debug("acceptor : [epoll_loop]. tq->push write_conn\n"); 
               }
            }
         }
      }
   }
   return true;
}
*/
