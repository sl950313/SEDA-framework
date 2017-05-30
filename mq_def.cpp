#include "mq_def.h"
#include <string.h>

using namespace std;

buffer *mq_def::encode_mq_msg(char *msg_content) {
   buffer *msg = new buffer();
   char style = 0x1;
   msg->push_back(&style, 1);
   msg->push_back(msg_content, strlen(msg_content));
   msg->push_back(&style, 1);
   return msg;
}

string mq_def::decode_mq_msg(buffer *msg) {
   string dec_msg = msg->ptr();
   //buffer *msg_content = new buffer();
   //msg_content->push_back(msg->ptr() + 1, msg->length() - 2);
   return dec_msg.substr(1, dec_msg.length() - 2);
}
