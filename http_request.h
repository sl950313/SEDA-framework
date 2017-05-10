#ifndef _HTTP_REQUEST_H
#define _HTTP_REQUEST_H
#include "buffer.h"
#include "struct.h"
#include <string>

class http_request {
public:
   read_http_status rhs;
   buffer *buf;
   http_request() : rhs(READ_HTTP_START), buf(new buffer()) {}
   ~http_request() {delete buf;}

   read_http_status read_http_status_machine(const char *_buf, int nread);

private:
   http_method method;
   http_proto proto;
   //std::string url;
   char url[64];
};

#endif //
