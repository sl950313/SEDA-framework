#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H 
#include "struct.h"
#include <string.h>
#include <map>
#include <string>

using namespace std;

typedef enum  { 
   CON_STATE_CONNECT,             //connect 连接开始  
   CON_STATE_REQUEST_START,     //reqstart 开始读取请求  
   CON_STATE_READ,             //read 读取并解析请求  
   CON_STATE_REQUEST_END,         //reqend 读取请求结束  
   CON_STATE_READ_POST,         //readpost 读取post数据  
   CON_STATE_HANDLE_REQUEST,     //handelreq 处理请求  
   CON_STATE_RESPONSE_START,     //respstart 开始回复  
   CON_STATE_WRITE,             //write 回复写数据  
   CON_STATE_RESPONSE_END,     //respend 回复结束  
   CON_STATE_ERROR,             //error 出错  
   CON_STATE_CLOSE             //close 连接关闭  

} connection_state_t;

struct http_head_t {
   http_method method;
   http_proto _proto;
   char url[64];
   read_http_status rhs;
   map<string, string> param;
};

struct http_body_t {
   char body[1024];
   http_body_t() {
      memset(body, 0, 1024);
   }
};

struct http_struct_t {
   struct http_head_t _head;
   struct http_body_t _body;
   connection_state_t _hstatus;
};

void *readHttpRequestStatusMachine(void *buf);
void *checkCache(void *);
void *cacheMiss(void *);
void *writeLog(void *);
void *writeHeadBody(void *); 

#endif /* _HTTP_SERVER_H */
