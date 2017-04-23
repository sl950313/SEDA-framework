#ifndef _STRUCT_H
#define _STRUCT_H

enum connect_status {
   CONNECT = 1,
   FINISH
};

struct connection {
   int fd;
   connect_status status;
   connection() {
      fd = -1;
      status = CONNECT;
   }
};

#endif ///
