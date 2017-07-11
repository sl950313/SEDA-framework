#include "loop.h"
#include "log.h"

void Loop::accept_conn() { 
   LogUtil::debug("acceptor : [accept_conn]\n");
   struct sockaddr_in actual_addr;
   int len = sizeof(struct sockaddr);
   int peer_sock = -1;
   while ((peer_sock = accept(s->getListenPort(), (sockaddr *)&actual_addr, (socklen_t *)&len)) > 0) {
      fcntl(peer_sock, F_SETFL, SOCK_NONBLOCK);
      struct epoll_event event;
      memset(&event, 0, sizeof(event));
      event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
      epoll_ctl(epfd, EPOLL_CTL_ADD, peer_sock, &event); 
   }

   if (peer_sock == -1) {
      if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR)
         perror("accept");
   }
   return NULL;
}

void Loop::read_msg() {
   int nread = -1;
   char buf[BUFSIZE] = { 0 };
   while ((nread = read(conn->fd, _buf, BUFSIZE - 1)) > 0) { 
      IElement *ie = 
   } 
}

void Loop::loop() { 
   int epfd = epoll_create1(EPOLL_CLOEXEC);
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
               read_msg();
               LogUtil::debug("acceptor : [epoll_loop]. tq->push read_conn");
            } else {
               if (events[i].events & EPOLLOUT) {
                  LogUtil::debug("acceptor : [epoll_loop]. tq->push write_conn"); 
               }
            }
         }
      }
   }
   return true;
}
