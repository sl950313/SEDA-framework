#include "acceptor.h"
#include "marcos.h"
#include "connect.h"
#include "struct.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>


acceptor::~acceptor() {
}

acceptor::acceptor() {
   port = DEFAULT_TCP_PORT;
   init_acceptor();
}

acceptor::acceptor(int _port) {
   port = _port;
   init_acceptor();
}

bool acceptor::init_acceptor() {
   int ret = 0;
   if ((ret = (listenfd = socket(AF_INET, SOCK_NONBLOCK, SOCK_STREAM))) == -1) {
      // log here.
      fprintf(stderr, "create socket error\n");
      return false;
   }
   struct sockaddr_in actual_addr;
   actual_addr.sin_family = AF_INET;
   actual_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   actual_addr.sin_port = htons(port);

   struct sockaddr *addr = (struct sockaddr *)&actual_addr;
   //addr.sa_family.
   if ((ret = bind(listenfd, addr, sizeof(actual_addr))) == -1) {
      fprintf(stderr, "bind socket error : %s\n", strerror(errno));
      return false;
   }

   return true;
}

connection *acceptor::accept_conn() {
   connection *conn = new connection();
   struct sockaddr_in actual_addr;
   int len = sizeof(struct sockaddr);
   int sock_fd = accept(listenfd, (sockaddr *)&actual_addr, (socklen_t *)&len);
   conn->fd = sock_fd;
   struct epoll_event event;
   event.events = EPOLLIN | EPOLLET;
   event.data.ptr = (void *)conn;
   epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &event);
   return conn;
}

bool acceptor::epoll_loop() {
   epfd = epoll_create1(EPOLL_CLOEXEC);
   struct epoll_event event;
   event.events = EPOLLIN | EPOLLET;
   event.data.fd = listenfd;
   epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);
   struct epoll_event events[MAXEVENTS];

   for (;;) {
      int rd_fds = epoll_wait(epfd, events, MAXEVENTS, 1000);
      for (int i = 0; i < rd_fds; ++i) {
         if (events[i].data.fd == listenfd) {
            connection *conn = accept_conn();
         } else {
            // TODO.
            if (events[i].events & EPOLLIN) {
               tq->push((void *)conn);
            }
         }
      }
   }
   return true;
}

void acceptor::start() {
   init_acceptor();
}
