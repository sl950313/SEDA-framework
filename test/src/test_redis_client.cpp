#include "../../redis_client.h"
#include <string.h>
#include "../../log.h"

int main() {
   const char *ip = "127.0.0.1";
   const char *command[] = {"set test:name shilei", "get test:name"};
   int port = 6379;

   LogUtil::init();
   redisClient *rc = new redisClient();
   rc->incrRefCount();
   bool suc = rc->connect(ip, port);
   if (suc) {
      printf("connect success\n");
   } else {
      printf("connect fail\n");
      rc->decrRefCount();
      return -1;
   }

   redisReply *r = rc->command(command[0]); 
   //redisReply *r = ((redisReply *)so->ptr);
   r = rc->command(command[1]);
   //r = (redisReply *)so->ptr;
   printf("test:name = %s\n", r->str);

   rc->decrRefCount();
   return 0;
}
