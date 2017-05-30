#ifndef _MQ_CONN_H
#define _MQ_CONN_H
#include "../buffer.h"
#include "message_queue_server.h"

enum mq_status {
   MQ_READ = 0,
   MQ_FINISH = 1,
   MQ_ERROR = 2
};

class message_queue_server;

class mq_conn {
public:
   mq_conn();
   ~mq_conn();
   void read_mq_msg_status_machine(char *_buf, int len);
   int fd;
   int epfd;
   message_queue_server *mqs;
   std::string addr;

private:
   buffer *buf;
   mq_status ms;
};

#endif //
