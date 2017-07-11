#ifndef _WORKER_POOL_H
#define _WORKER_POOL_H

#include "job_queue.h"
#include "struct.h"
#include "connect.h"
#include "function.h"

#include <pthread.h>
#include <vector>
#include <stdio.h>

#define DEFAULT_WORKER_NUM 8
typedef void * (*worker_task)(void *);

class worker_pool {
public:
   //worker_pool(Config &config);
   worker_pool() : worker_num(DEFAULT_WORKER_NUM) {worker_init_callback = NULL;}
   worker_pool(int _worker_num) : worker_num(_worker_num) {worker_init_callback = NULL;}
   worker_pool(int _worker_num , worker_task _worker_init_callback) : worker_num(_worker_num), worker_init_callback(_worker_init_callback) {worker_init_callback = NULL;}
   virtual ~worker_pool() {}

   virtual void start() {}
   void run(Function func);
   int worker_num;
   static void *per_worker_task(void *wp);
   bool running;
   void set_task_queue(task_queue *_jq) {jq = _jq;}
   worker_task worker_init_callback;

private:
   virtual void init();
   virtual void stop() {}
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
      connection *qe = (connection *)arg;
      //qe->status = FINISH;
      printf("%d : hello\n", qe->fd);
   }

private:
   void stop();
   std::vector<pthread_t> threads;
};

#endif //
