#ifndef _HTTP_REQUEST_H
#define _HTTP_REQUEST_H
#include "buffer.h"
#include "struct.h"
#include <string>
#include <string.h>
#include <map>

class http_request {
public:
   read_http_status rhs;
   buffer *buf;
   http_request() : rhs(READ_HTTP_START), buf(new buffer()) {
      memset(url, 0, 64);
   }
   ~http_request() {delete buf;}

   read_http_status read_http_status_machine(const char *_buf, int nread);
   void print_request_info();
   std::map<std::string, std::string> &get_map_contents();

private:
   http_method method;
   http_proto proto;
   //std::string url;
   char url[64];

   std::map<std::string, std::string> content;

   void get_params(char *content, int len);
   void get_params_from_url();
   void print_param();
};

#endif //
