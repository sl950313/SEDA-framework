#include "redis_client.h"
#include "log.h"

redisClient::~redisClient() {
   freeReplyObject(ptr);
   redisFree(c);
}

bool redisClient::connect(const char *ip, int port) {
   c = redisConnect(ip, port);
   if (c == NULL || c->err) {
      if (c) {
         LogUtil::debug("Error: %s", c->errstr);
         // handle error
      } else {
         LogUtil::debug("Can't allocate redis context");
      }
      redisFree(c);
      return false;
   }
   return true;
}

redisReply *redisClient::command(const char *format) {
   if (!c) {
      LogUtil::debug("This client is not connected. please involve the connect() first");
      return NULL;
   }
   ptr = redisCommand(c, format);
   return (redisReply *)ptr;
}
