#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define MAX_EVENT 1024

int main() {
   int ret, sock_fd;
   if ((ret = (sock_fd = socket(AF_INET, SOCK_STREAM, 0))) == -1) {
      printf("error socket : %s\n", strerror(errno));
      return -1;
   }
   struct sockaddr_in _addr;
   _addr.sin_family = AF_INET;
   _addr.sin_addr.s_addr = htonl(INADDR_ANY);
   _addr.sin_port = htons(6666);

   struct sockaddr *addr, addr_peer;
   addr = (struct sockaddr *)&_addr;

   fcntl(sock_fd, F_SETFL, SOCK_NONBLOCK);

   if ((ret = (bind(sock_fd, addr, sizeof(_addr)))) == -1) {
      printf("bind error : %s\n", strerror(errno));
      return -2;
   }

   listen(sock_fd, 50); 

   int len = sizeof(addr_peer);

   struct epoll_event event;
   struct epoll_event ready_event[MAX_EVENT];
   event.events = EPOLLIN | EPOLLET;
   event.data.fd = sock_fd;
   int epfd = epoll_create1(EPOLL_CLOEXEC);
   epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &event);
   while (1) {
      int ready_fds = epoll_wait(epfd, ready_event, MAX_EVENT, 1000);
      printf("ready_fds : %d\n", ready_fds);
      for (int i = 0 ; i < ready_fds; ++i) {
         if (ready_event[i].data.fd == sock_fd) { 
            int accept_fd = -1;
            while ((accept_fd = accept(sock_fd, &addr_peer, (socklen_t *)&len)) > 0) {
               printf("accept \n");
               struct epoll_event _event;
               _event.events = EPOLLIN | EPOLLET;
               _event.data.fd = accept_fd;
               fcntl(accept_fd, F_SETFL, SOCK_NONBLOCK);
               //sleep(5);
               epoll_ctl(epfd, EPOLL_CTL_ADD, accept_fd, &_event);
            }
            //write(accept_fd, "hi", 3);
         } else {
            printf("hello\n");
            if (ready_event[i].events & EPOLLIN) {
               char buf[2] = {0};
               read(ready_event[i].data.fd, buf, 1);
               printf("%c\n", buf[0]);
            }
         }
      }
   }

   return 0;
}
