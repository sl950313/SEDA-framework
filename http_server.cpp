#include "http_server.h"
#include "log.h"
#include "splaytree.h"
#include "stage.h"
#include <string.h>

void print_param(http_head_t &hht) {
   for (std::map<std::string, std::string>::iterator it = hht.param.begin(); it != hht.param.end(); ++it) {
      printf("%s : %s\n", it->first.c_str(), it->second.c_str());
   }
}

void get_params(http_head_t &hht, char *_content, int len) { 
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
      hht.param.insert(std::pair<std::string, std::string>(_p, _param_content));
      _p = strsep(&p, "&");
   }
}


void *readHttpRequestStatusMachine(void *_buf) {
   char *buf = (char *)_buf;
   int nread = strlen(buf);
   http_struct_t *hst = new http_struct_t();
   read_http_status rhs = hst->_head.rhs;
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
               hst->_head.method = GET;
            } else if (memcmp(buf + begin, "POST", strlen("POST")) == 0) {
               hst->_head.method = POST;
            } else {
               // error occur.
               LogUtil::debug("error content");
               rhs = READ_HTTP_ERROR;
            }
            begin = i + 1;
            rhs = URL;
         }
         break;
      case URL:
         //printf("URL\n");
         if (buf[i] == ' ') {
            memcpy(hst->_head.url, buf + begin, i - begin);
            if (hst->_head.method == GET) {
               LogUtil::debug("http_server.cpp : [read head]. url : %s", hst->_head.url);
               //get_params_from_url();
               //print_param();
            }
            begin = i + 1;
            rhs = PROTO;
         }
         break;
      case PROTO:
         //printf("PROT\n"); 
         if (buf[i] == '/') {
            if (memcmp(buf + begin, "HTTP", strlen("HTTP")) == 0 && ((i - begin == 4))) {
               hst->_head._proto = HTTP;
            } else if (memcmp(buf + begin, "HTTP", strlen("HTTP")) == 0 && ((i - begin == 5))) {
               hst->_head._proto = HTTPS;
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
            get_params(hst->_head, buf + i + 1, nread - i - 1);
            print_param(hst->_head);
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
   return (void *)hst;
}


int hashUrl(string &url) {
   return 0;
}

void *checkCache(void *arg) {
   http_struct_t *hst = (http_struct_t *)arg;
   int key = hashUrl(hst->_head.url); 
   splay_tree *root = stage::getPersistData("cache");
   splay_tree *sp = splaytree_splay(root, key);
   if (sp) { 
      memcpy(hst->_body.body, sp->data, strlen((char *)sp->data));
      return hst;
   } 
   char f_url[128] = { 0 };
   //memcpy(f_url, hst->_head.url, sizeof(hst->_head.url));
   sprintf(f_url, "/var/www/html/%s", hst->_head.url);
   LogUtil::debug("file %s will be response", f_url);

   return hst;
}

