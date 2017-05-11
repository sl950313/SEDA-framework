#include "http_request.h"
#include <string.h>
#include <stdio.h>

read_http_status http_request::read_http_status_machine(const char *_buf, int nread) {
   char *buf = (char *)_buf;
   int begin = 0;
   for (int i = 0; i < nread; ++i) {
      switch (rhs) {
      case READ_HTTP_START:
         rhs = METHORD;
      case METHORD:
         if (buf[i] == ' ') {
            if (memcmp(buf + begin, "GET", strlen("GET")) == 0) {
               method = GET;
            } else if (memcmp(buf + begin, "POST", strlen("POST")) == 0) {
               method = POST;
            } else {
               // error occur.
               printf("error content\n");
               rhs = READ_HTTP_ERROR;
            }
            begin = i + 1;
            rhs = URL;
         }
         break;
      case URL:
         if (buf[i] == ' ') {
            memcpy(url, buf + begin, i - begin);
            begin = i + 1;
            rhs = PROTO;
         }
         break;
      case PROTO:
         if (buf[i] == '/') {
            if (memcmp(buf + begin, "HTTP", strlen("HTTP")) == 0 && ((i - begin == 4))) {
               proto = HTTP;
            } else if (memcmp(buf + begin, "HTTP", strlen("HTTP")) == 0 && ((i - begin == 5))) {
               proto = HTTPS;
            }
            begin = i + 1;
            rhs = VERSION;
         }
         break;
      case VERSION:
         if (buf[i] == '\n') {
            //
            begin = i + 1;
            rhs = REQUEST_HEAD;
         }
         break;
      case REQUEST_HEAD:
         if (buf[i] == '\n') {
            begin = i + 1;
            if (buf[begin] == '\n') {
               rhs = CONTENT;
            }
         }
         break; 
      case CONTENT:
         if (buf[i] == '\n') {
            rhs = READ_HTTP_FINISH;
         } else { 
            char *p = buf + i;
            char *_p = strsep(&p, "&");
            while (_p) {
               char *param = strstr(_p, "=");
               if (!param) {
                  // error happen.
                  printf("error http request or request is not accpeted\n");
               }
               *param = '\0';
            }
            // TODO:
         }
         break;
      case READ_HTTP_FINISH:
         break;
      case READ_HTTP_ERROR:
         break;
      }
   }
   return rhs;
}

void http_request::print_request_info() {
   printf("Methord : %s, proto: %s, url : %s\n", (method == GET) ? "GET" : "POST", (proto == HTTP) ? "HTTP" : "HTTPS", url);
}
