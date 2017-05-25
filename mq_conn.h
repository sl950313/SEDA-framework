#ifndef _MQ_CONN_H
#define _MQ_CONN_H
#include "buffer.h"

enum mq_status {
   MQ_READ = 0,
   MQ_FINISH = 1,
   MQ_ERROR = 2
};

class mq_conn {
public:
   mq_conn();
   ~mq_conn();
   void read_mq_msg_status_machine(char *_buf, int len);
   int fd;
   int epfd;

private:
   buffer *buf;
   mq_status ms;
};

#endif //
