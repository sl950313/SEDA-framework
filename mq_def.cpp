#include "mq_def.h"

using namespace std;

buffer *mq_def::encode_mq_msg(buffer *msg_content) {
   buffer *msg = new buffer();
   char style = 0x1;
   msg->push_back(&style, 1);
   msg->push_back(msg_content->ptr(), msg_content->length());
   msg->push_back(&style, 1);
   return msg;
}

buffer *mq_def::decode_mq_msg(buffer *msg) {
   buffer *msg_content = new buffer();
   msg_content->push_back(msg->ptr() + 1, msg->length() - 2);
   return msg_content;
}
