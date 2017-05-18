#include "http_request.h"
#include <string.h>
#include <stdio.h>

read_http_status http_request::read_http_status_machine(const char *_buf, int nread) {
   char *buf = (char *)_buf;
   int begin = 0;
   for (int i = 0; i < nread; ++i) {
      switch (rhs) {
      case READ_HTTP_START:
         //printf("READ_HTTP_START\n");
         rhs = METHORD;
      case METHORD:
         //printf("METHORD\n");
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
         //printf("URL\n");
         if (buf[i] == ' ') {
            memcpy(url, buf + begin, i - begin);
            if (method == GET) {
               get_params_from_url();
               print_param();
            }
            begin = i + 1;
            rhs = PROTO;
         }
         break;
      case PROTO:
         //printf("PROT\n"); 
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
         //printf("VERSION\n");
         if (buf[i] == '\r' && buf[++i] == '\n') {
            //
            begin = i + 1;
            rhs = REQUEST_HEAD;
         }
         break;
      case REQUEST_HEAD:
         //printf("REQUEST_HEAD\n");
         //printf("%c  %d", buf[i], buf[i]);
         if (buf[i] == '\r' && buf[++i] == '\n') {
            begin = i + 1;
            if (buf[begin] == '\r' && buf[++begin] == '\n') {
               rhs = CONTENT;
               if (begin == nread - 1 || (begin == nread)) rhs = READ_HTTP_FINISH;
            }
         }
         break; 
      case CONTENT:
         //printf("CONTENT\n");
         if (buf[i] == '\n') {
         } else { 
            //printf("get content : \n");
            get_params(buf + i + 1, nread - i - 1);
            print_param();
         }
         rhs = READ_HTTP_FINISH;
         break;
      case READ_HTTP_FINISH:
         //printf("READ_HTTP_FINISH\n");
         break;
      case READ_HTTP_ERROR:
         //printf("READ_HTTP_ERROR\n");
         break;
      }
   }
   return rhs;
}

void http_request::print_param() {
   for (std::map<std::string, std::string>::iterator it = content.begin(); it != content.end(); ++it) {
      printf("%s : %s\n", it->first.c_str(), it->second.c_str());
   }
}

void http_request::get_params(char *_content, int len) { 
   char t_content[128] = {0};
   memcpy(t_content, _content, len);
   //printf("_contetn : %s\n", t_content);
   char *p = _content;
   char *_p = strsep(&p, "&");
   while (_p) {
      char *param = strstr(_p, "=");
      if (!param) {
         // error happen.
         printf("error http request or request is not accpeted\n");
      }
      *param = '\0';
      std::string _param = _p;
      std::string _param_content = param + 1;
      //printf("_param : %s, _param_content : %s\n", _p, _param_content.c_str());
      content.insert(std::pair<std::string, std::string>(_p, _param_content));
      _p = strsep(&p, "&");
   }
}

void http_request::get_params_from_url() {
   //printf("in get_params_from_url\n");
}

void http_request::print_request_info() {
   //printf("Methord : %s, proto: %s, url : %s\n", (method == GET) ? "GET" : "POST", (proto == HTTP) ? "HTTP" : "HTTPS", url);
}

std::map<std::string, std::string> &http_request::get_map_contents() {
   return content;
}
