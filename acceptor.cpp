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
   //count = 0;
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

void *acceptor::accept_conn(void) {
   //acceptor *ac = (acceptor *)conn->arg;
   log->debug("acceptor : [accept_conn]\n");
   //printf("accept_conn\n");
   struct sockaddr_in actual_addr;
   int len = sizeof(struct sockaddr);
   int peer_sock = -1;
   while ((peer_sock = accept(listenfd, (sockaddr *)&actual_addr, (socklen_t *)&len)) > 0) {
      connection *conn = new connection();
      conn->fd = peer_sock;
      conn->epfd = epfd;
      fcntl(peer_sock, F_SETFL, SOCK_NONBLOCK);
      struct epoll_event event;
      memset(&event, 0, sizeof(event));
      event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
      event.data.ptr = (void *)conn;
      epoll_ctl(epfd, EPOLL_CTL_ADD, peer_sock, &event); 
      //printf("accept fd : %d\n", conn->fd);
      
      // insert into map.
      //fd_conn.insert(std::pair<int, connection *>(peer_sock, conn));
   }

   if (peer_sock == -1) {
      if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR)
         perror("accept");
   }
   return NULL;
}

void *acceptor::read_conn(void *arg) {
   //printf("in read_conn.\n");
   connection *conn = (connection *)arg;
   int nread = -1;
   char _buf[BUFSIZE] = {0};
   while ((nread = read(conn->fd, _buf, BUFSIZE - 1)) > 0) {
      //_buf[nread - 1] = '\0';
      conn->req->buf->push_back(_buf, nread);
      conn->req->read_http_status_machine(_buf, nread);
      //printf("_buf : \n%s.\n", _buf);
      conn->req->print_request_info(); 
      struct epoll_event event;
      memset(&event, 0, sizeof(event));
      event.data.ptr = (void *)conn;
      if (conn->req->rhs == READ_HTTP_FINISH) {
         //printf("%lu, fd : %d. request has already been processed\n", pthread_self(), conn->fd);
         event.events = EPOLLOUT | EPOLLET | EPOLLONESHOT; 
         conn->set_user_info_to_handle();
      } else { 
         event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
      }
      int ret = epoll_ctl(conn->epfd, EPOLL_CTL_MOD, conn->fd, &event);
      if (ret == -1) fprintf(stderr, "epoll_ctl : %d EROLL_CTL_MOD EPOLL_OUT.", conn->fd);
   }
   //printf("%lu ---------------rhs : %d\n", pthread_self(), conn->req->rhs);
   if (nread == 0) {
      printf("%lu, %d read 0. client closed\n", pthread_self(), conn->fd);
      struct epoll_event event;
      memset(&event, 0, sizeof(event));
      int ret = epoll_ctl(conn->epfd, EPOLL_CTL_DEL, conn->fd, &event);
      if (ret == -1) {
         //perror("epoll_ctl : read_conn");
         printf("epoll_ctl : EPOLL_CTL_DEL : %s\n", strerror(errno));
      }

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
   connection *conn = (connection *)arg;
   //conn->rsp;
   std::string rsp_content = "I have already receive your request. please wait...";
   std::string rsp_head = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(rsp_content.length()) + "\r\n\r\n";
   std::string rsp = rsp_head + rsp_content;
   //std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(rsp_content.length()) + "\r\nI have already receive your request. please wait...";
   int nwrite = 0;
   int len = rsp.length();
   while (len > 0) { 
      nwrite = write(conn->fd, rsp.c_str() + rsp.length() - len, len);
      if (nwrite < len) {
         if (nwrite == -1 && errno != EAGAIN) perror("write error");
         if (nwrite == -1) break;
      }
      len -= nwrite;
      //printf("%lu nwrite : %d, len : %d\n", pthread_self(), nwrite, len);
   }
   //printf("%lu write to %d\n", (unsigned long)pthread_self(), conn->fd);
   //write(conn->fd, rsp_content.c_str(), rsp_content.length());
   //while ((nwrite = write(conn->fd, response.c_str(), response.length())) > 0) {
   //}

   // TODO

   if (FORCE_CLOSE) {
      struct epoll_event event;
      memset(&event, 0, sizeof(event));
      //event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
      event.data.ptr = (void *)conn;
      int ret = epoll_ctl(conn->epfd, EPOLL_CTL_DEL, conn->fd, &event); 
      if (ret == -1) {
         //perror("epoll_ctl : read_conn");
         printf("epoll_ctl : %d read_conn EPOLL_CTL_MOD. %s\n", conn->fd, strerror(errno));
      }
      close(conn->fd);
      delete conn;
      conn = NULL;
      return NULL;
   }

   struct epoll_event event;
   memset(&event, 0, sizeof(event));
   event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
   event.data.ptr = (void *)conn;
   int ret = epoll_ctl(conn->epfd, EPOLL_CTL_MOD, conn->fd, &event); 
   if (ret == -1) {
      //perror("epoll_ctl : read_conn");
      printf("epoll_ctl : %d read_conn EPOLL_CTL_MOD. %s\n", conn->fd, strerror(errno));
   }
   printf("%lu write_conn already success.\n", pthread_self());

   /*
    * 填写response并返回后, 不应该关闭该套接字, 然后再read_conn中删除epfd中的conn->fd.
    */
   //close(conn->fd);
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

void acceptor::start() {
   epoll_loop();
   //init_acceptor();
}
