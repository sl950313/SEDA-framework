#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include "job_queue.h"
#include "struct.h"

class acceptor {
public:
   acceptor();
   acceptor(int _port);
   ~acceptor();
   void start();
   void set_task_queue(task_queue *_tq) {tq = _tq;}

public: 
   bool init_acceptor();
   bool epoll_loop();
   connection *accept_conn();

   task_queue *tq;
   int listenfd;
   int port;
   int epfd;
};

#endif //
