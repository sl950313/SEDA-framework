#include "worker_pool.h"
#include "stdlib.h"
#include "log.h"

void worker_pool::run(Function func) {
   queue_element *qe = new queue_element(func.getFunction(), func.getArg());
   jq->push(qe);
}

/*
 * Create the task queue for thread pool
 * Thus the worker pool can run like this:
 * 
 * void *foo(void *) {
 *    printf("Hello World\n");
 *    return NULL;
 * }
 * worker_task wt;
 * worker_pool = new thread_worker_pool(Config &config);
 * worker_pool.run(wt);
 * worker_pool.run(wt);
 */
void worker_pool::init() {
   jq = new mutex_task_queue();
   running_num = 0;
   pthread_mutex_init(&statics_lock, NULL);
}

void *worker_pool::per_worker_task(void *arg) {
   worker_pool *wp = (worker_pool *)arg;
   /*
   if (wp->worker_init_callback) {
      wp->worker_init_callback(NULL);
   } 
   */
   while (wp->running) {
      queue_element* qe = (queue_element *)wp->jq->pop();

      pthread_mutex_lock(&wp->statics_lock);
      wp->running_num++;
      pthread_mutex_unlock(&wp->statics_lock);

      if (wp->worker_init_callback) {
         wp->worker_init_callback(NULL);
      } else {
         //wp->exe_work((void *)qe);
         qe->_cb(qe->arg);
         delete qe;
      }
      pthread_mutex_lock(&wp->statics_lock);
      wp->running_num--;
      pthread_mutex_unlock(&wp->statics_lock);
      //TODO
      //wp->worker_init_callback(qe);
   }
   return NULL;
}

int worker_pool::getRunnningWorkerNum() {
   /*
    * Here we can not do decision based on a current value of running num.
    * as the value is changing all the time.
    */
   return running_num;
}

void thread_worker_pool::start() {
   threads.resize(worker_num);
   running = true;
   LogUtil::debug("worker_pool : [start], worker_num : %d", worker_num);
   for (int i = 0; i < worker_num; ++i) {
      pthread_create(&(threads[i]), NULL, per_worker_task, this);
   }
}

void thread_worker_pool::stop() {
   void *status;
   for (int i = 0; i < worker_num; ++i) {
      pthread_join(threads[i], &status);
   }
}
