#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main(int argc, char **argv) {
   int i = 0;
   for (i = 0; i < atoi(argv[argc - 1]); ++i) {
      if (fork() == 0) {
         break;
      }
   }
   printf("process id : %d\n", i);

   int ret, sock_fd;
   if ((ret = (sock_fd = socket(AF_INET, SOCK_STREAM, 0))) == -1) {
      printf("error socket : %s\n", strerror(errno));
      return -1;
   }
   struct sockaddr_in _addr_client, _addr;
   _addr.sin_family = AF_INET;
   _addr.sin_addr.s_addr = htonl(INADDR_ANY);
   _addr.sin_port = htons(6666);

   _addr_client.sin_family = AF_INET;
   _addr_client.sin_addr.s_addr = inet_addr("114.214.169.173");
   _addr_client.sin_port = htons(6666);

   struct sockaddr *addr;
   addr = (struct sockaddr *)&_addr; 
   socklen_t socklen = sizeof(_addr); 

   if ((ret = connect(sock_fd, (struct sockaddr *)&_addr_client, socklen)) == -1) {
      printf("error connect : %s\n", strerror(errno));
      return -1;
   }
   printf("connect success : %d\n", i);
   for (int i = 0; i < 5; ++i) {
      int wl = write(sock_fd, "12345", 4);
      printf("write %d bytes\n", wl);
      sleep(2); 
   }

   close(sock_fd);
   return 0;
}
