#ifndef _STRUCT_H
#define _STRUCT_H

typedef char byte;

enum connect_status {
   CONNECT = 1,
   READ_HTTP,
   RESOLVE_HTTP,
   HANDLE,
   RESPONSE,
   FINISH
};

enum read_http_status {
   READ_HTTP_START = 0,
   METHORD = 1 << 1,
   URL = 1 << 2,
   PROTO = 1 << 3,
   VERSION = 1 << 4,
   REQUEST_HEAD = 1 << 5,
   CONTENT = 1 << 6,
   READ_HTTP_FINISH = 1 << 7,
   READ_HTTP_ERROR = 1 << 8
};

enum http_status {
   INITIAL = 1,
   HTTP_FINISH
};

enum http_method {
   GET = 1,
   POST
};

enum http_proto {
   HTTP = 1,
   HTTPS
};

/*
struct user_info {
};
*/
typedef void *(*function)(void *);

#endif ///
