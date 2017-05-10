#include "acceptor.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h> 
#include <unistd.h>

acceptor::~acceptor() {
}

acceptor::acceptor() {
   port = DEFAULT_TCP_PORT;
   init_acceptor();
}

acceptor::acceptor(_log *log) {
   this->log = log;
   port = DEFAULT_TCP_PORT;
   init_acceptor();
   this->log->info("acceptor : [acceptor(_log *)]\n");
}

acceptor::acceptor(int _port, _log *log) {
   port = _port;
   init_acceptor();
   this->log = log;
}

bool acceptor::init_acceptor() {
   log->debug("acceptor : [init_acceptor]\n");
   int ret = 0;
   if ((ret = (listenfd = socket(AF_INET, SOCK_STREAM, 0))) == -1) {
      // log here.
      fprintf(stderr, "create socket error\n");
      return false;
   }
   struct sockaddr_in actual_addr;
   actual_addr.sin_family = AF_INET;
   actual_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   actual_addr.sin_port = htons(port);

   // set the listenfd socket to be nonblock.
   fcntl(listenfd, F_SETFL, SOCK_NONBLOCK);

   struct sockaddr *addr = (struct sockaddr *)&actual_addr;
   //addr.sa_family.
   if ((ret = bind(listenfd, addr, sizeof(actual_addr))) == -1) {
      fprintf(stderr, "bind socket error : %s\n", strerror(errno));
      return false;
   } 
   listen(listenfd, 50);
   return true;
}

void *acceptor::accept_conn(void) {
   //acceptor *ac = (acceptor *)conn->arg;
   log->debug("acceptor : [accept_conn]\n");
   struct sockaddr_in actual_addr;
   int len = sizeof(struct sockaddr);
   int peer_sock = -1;
   while ((peer_sock = accept(listenfd, (sockaddr *)&actual_addr, (socklen_t *)&len)) > 0) {
      connection *conn = new connection();
      conn->fd = peer_sock;
      fcntl(peer_sock, F_SETFL, SOCK_NONBLOCK);
      struct epoll_event event;
      event.events = EPOLLIN | EPOLLET;
      event.data.ptr = (void *)conn;
      epoll_ctl(epfd, EPOLL_CTL_ADD, peer_sock, &event); 
      
      // insert into map.
      fd_conn.insert(std::pair<int, connection *>(peer_sock, conn));
   }

   if (peer_sock == -1) {
      if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR)
         perror("accept");
   }
   return NULL;
}

void *acceptor::read_conn(void *arg) {
   printf("in read_conn.\n");
   connection *conn = (connection *)arg;
   int nread = -1;
   char _buf[BUFSIZE];
   while ((nread = read(conn->fd, _buf, BUFSIZE - 1)) > 0) {
      conn->req->buf->push_back(_buf, nread);
      conn->req->read_http_status_machine(_buf, nread);
   }
   printf("nread : %d\n", nread);
   if (nread == 0) {
      printf("client closed\n");
      close(conn->fd);
      delete conn;
      conn = NULL;
      /*
       * TODO.
       * here we do not delete the conn and set the status as CLIENT_CLOSE. the conn is deleted in worker_pool. because it can check the status of conn which has been already seted as CLIENT_CLOSE.
       */
   }
   if (nread == -1 && errno != EAGAIN) {
      // log error.
   }
   return NULL;
}

void *acceptor::write_conn(void *arg) {
   return NULL;
}

bool acceptor::epoll_loop() {
   epfd = epoll_create1(EPOLL_CLOEXEC);
   struct epoll_event event;
   memset(&event, 0, sizeof(event));
   event.events = EPOLLIN | EPOLLET;
   event.data.fd = listenfd;
   epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);
   struct epoll_event events[MAXEVENTS];

   //printf("acceptor looping\n");
   log->info("acceptor looping\n");

   for (;;) {
      int rd_fds = epoll_wait(epfd, events, MAXEVENTS, 1000);
      for (int i = 0; i < rd_fds; ++i) {
         if (events[i].data.fd == listenfd) {
            accept_conn();
         } else {
            // TODO.
            if (events[i].events & EPOLLIN) {
               connection *conn = (connection *)events[i].data.ptr;
               conn->cb = read_conn;
               tq->push((void *)conn);
               log->debug("acceptor : [epoll_loop]. tq->push read_conn\n");
            } else {
               if (events[i].events & EPOLLOUT) {
                  connection *conn = (connection *)events[i].data.ptr;
                  conn->cb = write_conn;
                  tq->push((void *)conn); 
                  log->debug("acceptor : [epoll_loop]. tq->push write_conn\n"); 
               }
            }
         }
      }
   }
   return true;
}

void acceptor::start() {
   epoll_loop();
   //init_acceptor();
}
