#ifndef _SERVER_H
#define _SERVER_H

#include "worker_pool.h"
#include "job_queue.h"
#include "connect.h"
#include "log.h"
#include "acceptor.h"

#include <string>

typedef void * (*fd_process)(void *);
class tcp_server {
public:
   tcp_server();
   tcp_server(std::string name = "tcp_server");
   virtual ~tcp_server();
   void loop();

   connector *con;
   fd_process fd_process_callback;
   
   _log log;

private:
   virtual void start();
   void init();

   std::string server_name;
   worker_pool *wp;
   task_queue *tq;
   acceptor *ac;
   static void *process_connection(void *arg);
   //bool init_tcp_server();
};

class multi_process_tcp_server : public tcp_server {
};

#endif // 
