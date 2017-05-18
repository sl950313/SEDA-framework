#ifndef _CONNECT_H
#define _CONNECT_H
#include "http_request.h"
#include "http_response.h"
#include "handle_info.h"

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
   handle_info *hi;

   connect_status status;
   void *arg;
   conn_callback cb;

   connection() {
      fd = -1;
      status = CONNECT;
      req = new http_request();
      rsp = new http_response();
      hi = new handle_info();
   }
   ~connection() {
      delete req;
      delete rsp;
      //if (arg) free(arg);
   }

   void set_user_info_to_handle();
};

#endif //
