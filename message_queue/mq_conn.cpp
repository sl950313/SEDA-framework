#include "mq_conn.h"

mq_conn::mq_conn() {
   buf = new buffer();
   fd = -1;
   epfd = -1;
}

mq_conn::~mq_conn() {
   delete buf;
}

void mq_conn::read_mq_msg_status_machine(char *_buf, int len) { 
   int msg_style_begin = _buf[0];
   int msg_style_end = _buf[len - 1];
   if (msg_style_end != msg_style_begin) { 
      ms = MQ_ERROR;
   } else {
      ms = MQ_FINISH;
   }
}
