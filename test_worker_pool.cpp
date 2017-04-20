#include "worker_pool.h"
#include "job_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *write_queue(void *arg) {
   task_queue *tq = (task_queue *)arg;
   int i = 0;
   while (true) {
      queue_element *qe = new queue_element();
      qe->fd = ++i;
      tq->push(qe);
      usleep(100);
   }
   return NULL;
}

int main() { 
   task_queue *tq = new mutex_task_queue();
   printf("push ok\n");
   //sleep(1);

   pthread_t write_handle;
   pthread_create(&write_handle, NULL, write_queue, (void *)tq);

   worker_pool *wp = new thread_worker_pool();
   wp->set_task_queue(tq);
   printf("worker_pool created\n");
   wp->start();
   printf("worker_pool running\n");
   //printf("task execute finish\n");
   delete wp;
}
