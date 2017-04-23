#ifndef _WORKER_POOL_H
#define _WORKER_POOL_H

#include "job_queue.h"
#include <pthread.h>
#include <vector>
#include <stdio.h>

#define DEFAULT_WORKER_NUM 8
typedef void * (*worker_task)(void *);

class worker_pool {
public:
   worker_pool() : worker_num(DEFAULT_WORKER_NUM) {}
   worker_pool(int _worker_num) : worker_num(_worker_num) {}
   worker_pool(int _worker_num , worker_task _worker_init_callback) : worker_num(_worker_num), worker_init_callback(_worker_init_callback) {}
   virtual ~worker_pool() {}

   virtual void start() {}
   virtual void stop() {}
   void run(worker_task task);
   int worker_num;
   static void *per_worker_task(void *wp);
   bool running;
   void set_task_queue(task_queue *_jq) {jq = _jq;}
   worker_task worker_init_callback;

private:
   virtual void exe_work(void *arg) {}
   task_queue *jq;

};

class thread_worker_pool : public worker_pool {
public:
   thread_worker_pool() {
      //worker_init_callback = &exe_work;
   }
   ~thread_worker_pool() {stop();}
   
   void start();
   void exe_work(void *arg) {
      queue_element *qe = (queue_element *)arg;
      qe->_status = FINISH;
      printf("hello\n");
   }
private:
   void stop();
   std::vector<pthread_t> threads;
};

#endif //
