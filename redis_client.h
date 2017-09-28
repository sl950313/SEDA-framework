#ifndef REDIS_CLIENT_H
#define REDIS_CLIENT_H
#include <hiredis/hiredis.h>
#include "seda_object.h"

class redisClient : public sedaObject {
public:
   bool connect(const char *ip, int port);
   redisReply *command(const char *format); 
   ~redisClient();

private:
   redisContext *c;
   //sedaObject *r;
};

#endif 
