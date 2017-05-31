#include "message_queue/message_queue_client.h"
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

void *test_run(void *arg) {
   char *msg = (char *)arg;
   printf("recv msg from server: %s\n", msg);
   return NULL;
}

void *publish(void *arg) {
   mq_client *mc = (mq_client *)arg;
   time_t _time;
   int i = 0;
   while (i < 100) {
      sleep(1);
      time(&_time);
      mc->send(asctime(gmtime(&_time)));
   }
   return NULL;
}

void handle_error(char *msg) {
   printf("error occur : %s\n", msg);
   fflush(stdout);
   exit(-1);
}

int main(int argc, char **argv) { 
   //string ser_ip_port = "114.214.169.173:16668";
   if (argc < 3) {
      printf("./test_mq_client ip:port role\n");
      return -1;
   }
   string ser_ip_port = argv[1];
   int role = atoi(argv[2]);
   int ret = -1;
   mq_client *mc = new mq_client(ser_ip_port);
   ret = mc->connect();
   if (ret) handle_error((char *)"connect");
   char publisher_name[128] = {0};
   if (role) {
      size_t index = ser_ip_port.find(':');
      memcpy(publisher_name, ser_ip_port.c_str(), index);
   }
   ret = mc->register_mq(role, publisher_name);
   if (ret) handle_error((char *)"register_mq");
   mc->loop(test_run);
   pthread_t pid = 0;
   void *retu = NULL;
   if (!role) {
      pthread_create(&pid, NULL, publish, mc);
   }
   delete mc;
   if (role) pthread_join(pid, &retu);
   return 0;
}


