#include "../worker_pool.h"
#include "../job_queue.h"

#include <stdio.h>

int main() {
   worker_pool *wp = new thread_worker_pool();
   //wp->worker_num = 4;
   wp->start();
   printf("worker_pool running\n");

   task_queue *tq = new mutex_task_queue();
   queue_element *qe[4];
   for (int i = 0; i < 4; ++i) {
      queue_element *qe_t = new queue_element();
      qe[i] = qe_t;
      qe[i]->fd = i;
      tq->push(qe[i]);
   }

   printf("task execute finish\n");
   for (int i = 0; i < 4; ++i) {
      delete qe[i];
   }
}
