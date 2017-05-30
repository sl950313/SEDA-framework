#ifndef _MQ_DEF_H
#define _MQ_DEF_H
#include "buffer.h"
#include <string>

class mq_def {
public:
   static buffer *encode_mq_msg(char *msg_content);
   static std::string decode_mq_msg(buffer *msg);
};

#endif //_MQ_DEF_H
