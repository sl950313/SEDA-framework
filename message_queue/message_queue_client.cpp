#include "message_queue_client.h"
#include "../marcos.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

mq_client::mq_client(std::string ip_port) {
   server_ip_port = ip_port;
}

mq_client::~mq_client() {
   wait();
}

int mq_client::connect() {
   std::string server_ip;
   int server_port;
   int ret = get_ip_port(server_ip_port, server_ip, server_port);
   if (!ret) {}// error input.
   fd = socket(AF_INET, SOCK_STREAM, 0);
   struct sockaddr_in addr;
   memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_port = htons(server_port);
   inet_pton(AF_INET, server_ip.c_str(), &addr.sin_addr);
   ret = ::connect(fd, (struct sockaddr *)&addr, sizeof(addr));
   if (ret < 0) {return -1;}
   return 0;
}

int mq_client::register_mq(int role, char *publisher_name) {
   char register_msg[128] = {0};
   if (role == 0) { register_msg[0] = 0x2; }
   if (role == 1) {
      register_msg[0] = 0x4; 
      sprintf(register_msg + 1, "%s", publisher_name);
   }
   int len = strlen(register_msg);
   for (int i = 0; i < len; ++i) {
      register_msg[len + 1] += register_msg[i];
   }

   return (write(fd, register_msg, len + 1) == len + 1) ? 0 : 1;
}

int mq_client::unregister_mq(int role, char *publisher_name) {
   char register_msg[128] = {0};
   if (role == 0) register_msg[0] = 0x8;
   if (role == 1) {
      register_msg[0] = 0x16;
      sprintf(register_msg + 1, "%s", publisher_name);
   }
   int len = strlen(register_msg);
   for (int i = 0; i < len; ++i) {
      register_msg[len + 1] += register_msg[i];
   }
   return write(fd, register_msg, len + 1) == len + 1;
}

void mq_client::loop(callback cb) {
   _cb = cb;
   pthread_create(&pid, NULL, run, this);
}

void mq_client::wait() {
   //pthread_kill(pid, SIGKILL);
   void *ret = NULL;
   pthread_join(pid, &ret);
}

void mq_client::sig_handle(int sig, siginfo_t *st, void *ret) {

}

void *mq_client::run(void *arg) {
   printf("Here running\n");
   //signal(SIGKILL, sig_handle);
   /*
   struct sigaction act, oact;
   //act. sa_handler = sig_handle;
   act.sa_sigaction = sig_handle;
   act.sa_flags = SA_SIGINFO;
   sigemptyset(&act.sa_mask); //清空此信号集
   act. sa_flags = 0;
   sigaction(SIGKILL, &act, &oact);
   */
   mq_client *mc = (mq_client *)arg;
   char buf[BUFSIZE] = {0};
   while (true) {
      memset(buf, 0, BUFSIZE);
      int nread = read(mc->fd, buf, BUFSIZE);
      if (nread == 0) {
         printf("server close the socket.\n");
         break;
      } 
      if (nread == -1) {
         printf("error may occur. error str : %s\n", strerror(errno));
         break;
      }
      mc->_cb(buf);
   }
   return NULL;
}

int mq_client::send(std::string msg) {
   char send_msg[128] = {0};
   send_msg[0] = 0x1;
   sprintf(send_msg + 1, msg.c_str(), msg.length());
   for (size_t i = 0; i < msg.length() + 1; ++i) {
      send_msg[msg.length() + 1] += send_msg[i];
   }
   int nwrite = write(fd, send_msg, msg.length() + 2);
   if (nwrite - 2 != (int)msg.length()) {return -1;}
   return 0;
}

int mq_client::get_ip_port(std::string &server_ip_port, std::string &server_ip, int &port) {
   int index = server_ip_port.find(':');
   server_ip = server_ip_port.substr(0, index);
   port = atoi(server_ip_port.c_str() + index + 1);
   if (port == -1) return -1;
   return 0;
}


