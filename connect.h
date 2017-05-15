#ifndef _CONNECT_H
#define _CONNECT_H
#include "http_request.h"
#include "http_response.h"

class connector {
};

typedef void * (*conn_callback)(void *);
class connection {
public:
   int fd;
   int epfd;
   http_status hs;
   http_request *req;
   http_response *rsp;
   connect_status status;
   void *arg;
   conn_callback cb;

   connection() {
      fd = -1;
      status = CONNECT;
      req = new http_request();
      rsp = new http_response();
   }
   ~connection() {
      delete req;
      delete rsp;
      //if (arg) free(arg);
   }
};

#endif //
