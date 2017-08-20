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
#include <string>
#include <sys/sendfile.h>
#include <sys/stat.h>

using namespace std;

acceptor::~acceptor() {
}

acceptor::acceptor() {
   port = DEFAULT_TCP_PORT;
   init_acceptor();
   init_worker_pool();
}

acceptor::acceptor(int _port) {
   port = _port;
   init_acceptor();
   init_worker_pool();
}

bool acceptor::init_worker_pool() {
   wp = new thread_worker_pool();
   wp->start();
   return true;
}

bool acceptor::init_acceptor() {
   LogUtil::debug("acceptor : [init_acceptor]");
   int ret = 0;
   if ((ret = (listenfd = socket(AF_INET, SOCK_STREAM, 0))) == -1) {
      // log here.
      LogUtil::debug("create socket error");
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
   struct sockaddr_in actual_addr;
   int len = sizeof(struct sockaddr);
   int peer_sock = -1;
   while ((peer_sock = accept(listenfd, (sockaddr *)&actual_addr, (socklen_t *)&len)) > 0) {
      connection *conn = new connection();
      conn->fd = peer_sock;
      conn->epfd = epfd;
      LogUtil::debug("acceptor : [accept_conn] fd=%d", conn->fd);
      fcntl(peer_sock, F_SETFL, SOCK_NONBLOCK);
      struct epoll_event event;
      memset(&event, 0, sizeof(event));
      event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
      event.data.ptr = (void *)conn;
      epoll_ctl(epfd, EPOLL_CTL_ADD, peer_sock, &event); 
      
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
   connection *conn = (connection *)arg;
   int nread = -1;
   char _buf[BUFSIZE] = {0};
   while ((nread = read(conn->fd, _buf, BUFSIZE - 1)) > 0) {
      conn->req->buf->push_back(_buf, nread);
      conn->req->read_http_status_machine(_buf, nread);
      conn->req->print_request_info(); 
      struct epoll_event event;
      memset(&event, 0, sizeof(event));
      event.data.ptr = (void *)conn;
      if (conn->req->rhs == READ_HTTP_FINISH) {
         event.events = EPOLLOUT | EPOLLET | EPOLLONESHOT; 
         conn->set_user_info_to_handle();
      } else { 
         event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
      }
      int ret = epoll_ctl(conn->epfd, EPOLL_CTL_MOD, conn->fd, &event);
      if (ret == -1) fprintf(stderr, "epoll_ctl : %d EROLL_CTL_MOD EPOLL_OUT.", conn->fd);
   }
   if (nread == 0) {
      LogUtil::debug("%d read 0. client closed", conn->fd);
      struct epoll_event event;
      memset(&event, 0, sizeof(event));
      int ret = epoll_ctl(conn->epfd, EPOLL_CTL_DEL, conn->fd, &event);
      if (ret == -1) {
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

void *acceptor::writeBody(string &rsp_content, connection *conn) {
   char filename[64] = {0};
   char *url = conn->req->getUrl();
   int len = strlen(url);
   sprintf(filename, "/var/www/html%s", (len == 1) ? "/index.html" : url);
   FILE *fp = fopen(filename, "r");
   if (!fp) {
      LogUtil::debug("file %s open error. %s", filename, strerror(errno));
      return NULL;
   } 
   char buf[1024] = {0};
   while (1) {
      int nread = fread(buf, 1, 1023, fp); 
      if (nread == 0) {
         break;
      }
      rsp_content += buf;
   }
   fclose(fp);

   return NULL;
}

void *acceptor::write_conn(void *arg) { 
   connection *conn = (connection *)arg;
   //conn->rsp;
   LogUtil::debug("acceptor : write_conn, url=%s", conn->req->getUrl());
   if (!USE_SENDFILE) {
      std::string rsp_content = "I have already receive your request. please wait...";
      writeBody(rsp_content, conn);
      std::string rsp_head = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(rsp_content.length()) + "\r\n\r\n";
      std::string rsp = rsp_head + rsp_content;

      int nwrite = 0;
      int len = rsp.length();
      while (len > 0) { 
         nwrite = write(conn->fd, rsp.c_str() + rsp.length() - len, len);
         if (nwrite < len) {
            if (nwrite == -1 && errno != EAGAIN) perror("write error");
            if (nwrite == -1) break;
         }
         len -= nwrite;
      }
   } else {
      char filename[64] = {0};
      char *url = conn->req->getUrl();
      int len = strlen(url);
      sprintf(filename, "/var/www/html%s", (len == 1) ? "/index.html" : url);
      int fd = open(filename, O_RDONLY);
      if (fd <= 0) {
         LogUtil::debug("file %s open error. %s", filename, strerror(errno));
         return NULL;
      }
      struct stat stat_buf;
      fstat(fd, &stat_buf);
      int nwrite = sendfile(conn->fd, fd, NULL, stat_buf.st_size);
   }

   LogUtil::debug("write_conn : write success");
   // TODO
   // 关闭单工通道.
   //close(conn->fd);

   if (FORCE_CLOSE) {
      struct epoll_event event;
      memset(&event, 0, sizeof(event));
      //event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
      event.data.ptr = (void *)conn;
      int ret = epoll_ctl(conn->epfd, EPOLL_CTL_DEL, conn->fd, &event); 
      if (ret == -1) {
         //perror("epoll_ctl : read_conn");
         printf("epoll_ctl_1 : %d read_conn EPOLL_CTL_MOD. %s\n", conn->fd, strerror(errno));
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
      LogUtil::debug("epoll_ctl_2 : %d read_conn EPOLL_CTL_MOD. %s\n", conn->fd, strerror(errno));
   }

   return NULL;
}

bool acceptor::epoll_loop() {
   //fork();
   epfd = epoll_create1(EPOLL_CLOEXEC);
   struct epoll_event event;
   memset(&event, 0, sizeof(event));
   event.events = EPOLLIN | EPOLLET;
   event.data.fd = listenfd;
   epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);
   struct epoll_event events[MAXEVENTS];

   LogUtil::debug("acceptor looping");

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
               Function func(conn->cb, conn);
               wp->run(func);
               LogUtil::debug("acceptor : [epoll_loop]. tq->push read_conn %d", conn->fd);
            } else {
               if (events[i].events & EPOLLOUT) {
                  connection *conn = (connection *)events[i].data.ptr;
                  conn->cb = write_conn;
                  Function func(conn->cb, conn);
                  wp->run(func);
                  LogUtil::debug("acceptor : [epoll_loop]. tq->push write_conn %d", conn->fd); 
               }
            }
         }
      }
   }
   return true;
}

void acceptor::start() {
   epoll_loop();
}
