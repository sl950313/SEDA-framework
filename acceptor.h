#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H
#include <map>
#include <string>
#include "job_queue.h"
#include "struct.h"
#include "http_module.h"
#include "http_request.h"
#include "connect.h"
#include "marcos.h"
#include "connect.h"
#include "log.h"
#include "worker_pool.h"

class acceptor {
public:
   acceptor();
   acceptor(int _port);
   ~acceptor();
   void start();
   //void set_task_queue(task_queue *_tq) {tq = _tq;}

private: 
   bool init_acceptor();
   bool init_worker_pool();
   bool epoll_loop();
   void *accept_conn(void);
   static void *read_conn(void *);
   static void *write_conn(void *);
   static void *writeBody(std::string &body, connection *conn);

   std::map<int, connection *> fd_conn;

   http_module *ha;
   task_queue *tq;
   int listenfd;
   int port;
   int epfd;

   worker_pool *wp;
   int count;
};

#endif //
