#include "loop.h"
#include "log.h"
#include "buffer.h"
#include "IElement.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

void *Loop::accept_conn() { 
   LogUtil::debug("acceptor : [accept_conn]\n");
   struct sockaddr_in actual_addr;
   int len = sizeof(struct sockaddr);
   int peer_sock = -1;
   while ((peer_sock = accept(s->getListenfd(), (sockaddr *)&actual_addr, (socklen_t *)&len)) > 0) {
      fcntl(peer_sock, F_SETFL, SOCK_NONBLOCK);
      struct epoll_event event;
      memset(&event, 0, sizeof(event));
      event.data.fd = peer_sock;
      event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
      epoll_ctl(s->getEpfd(), EPOLL_CTL_ADD, peer_sock, &event); 
   }

   if (peer_sock == -1) {
      if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR)
         perror("accept");
   }
   return NULL;
}

void *Loop::read_conn(void *arg) {
   int peer_fd = (int )arg;
   int nread = -1;
   char _buf[BUFSIZE] = { 0 };
   buffer *buf = new buffer();
   while ((nread = read(peer_fd, _buf, BUFSIZE - 1)) > 0) { 
      buf->push_back(_buf, nread);
   }
   IElement ie(buf->ptr());
   sq->push(ie);

   if (nread == 0) {
      LogUtil::debug("client closed");
      struct epoll_event event;
      memset(&event, 0, sizeof(event));
      epoll_ctl(s->getEpfd(), EPOLL_CTL_DEL, peer_fd, &event);
      close(peer_fd);
   }
   return NULL;
}

void Loop::loop() { 
   int epfd = epoll_create1(EPOLL_CLOEXEC);
   struct epoll_event event;
   memset(&event, 0, sizeof(event));
   event.events = EPOLLIN | EPOLLET;
   event.data.fd = s->getListenfd();
   epoll_ctl(epfd, EPOLL_CTL_ADD, s->getListenfd(), &event);
   struct epoll_event events[MAXEVENTS];
   //log->info("acceptor looping\n");
   LogUtil::debug("Socket : [loop] running");

   for (;;) {
      int rd_fds = epoll_wait(epfd, events, MAXEVENTS, 1000);
      for (int i = 0; i < rd_fds; ++i) {
         if (events[i].data.fd == s->getListenfd()) {
            //acceptor *ac = this;
            accept_conn();
         } else {
            // TODO.
            if (events[i].events & EPOLLIN) {
               read_conn((void *)&(events[i].data.fd));
               //read_msg();
               LogUtil::debug("acceptor : [epoll_loop]. tq->push read_conn");
            } else {
               if (events[i].events & EPOLLOUT) {
                  LogUtil::debug("acceptor : [epoll_loop]. tq->push write_conn"); 
               }
            }
         }
      }
   }
   //return true;
}
