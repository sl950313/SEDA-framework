#include "message_queue/message_queue_client.h"
#include <string>

using namespace std;

void *test_run(void *arg) {
   char *msg = (char *)arg;
   printf("recv msg from server: %s\n", msg);
   return NULL;
}

int main(int argc, char **argv) { 
   //string ser_ip_port = "114.214.169.173:16668";
   if (argc < 3) {
      printf("./test_mq_client ip:port role\n");
      return -1;
   }
   string ser_ip_port = argv[1];
   int role = atoi(argv[2]);
   mq_client *mc = new mq_client(ser_ip_port);
   mc->connect();
   mc->register_mq(role);
   mc->loop(test_run);
   return 0;
}


