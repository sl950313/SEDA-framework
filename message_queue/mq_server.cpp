#include "message_queue_server.h"

int main() {
   message_queue_server *mqs = new message_queue_server();
   mqs->init_mq();
   //delete mqs;
}
