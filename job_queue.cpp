#include "job_queue.h"
#include <stdio.h>

task_queue::task_queue() {
   empty = true;
   full = false;
   max_queue_size = 2000;
}

bool task_queue::is_full() {
   return full;
}

bool task_queue::is_empty() {
   return empty;
}

std::queue<void *>* task_queue::get_queue() {
   return &_queue;
}

mutex_task_queue::mutex_task_queue() {
   pthread_mutex_init(&queue_mutex, NULL);
   pthread_cond_init(&not_full, NULL);
   pthread_cond_init(&not_empty, NULL);
}

void mutex_task_queue::push(void *qe) {
   pthread_mutex_lock(&queue_mutex);
   while (full) {
      pthread_cond_wait(&not_full, &queue_mutex);
   }
   std::queue<void *>* _queue = get_queue();
   _queue->push(qe);
   if (empty) pthread_cond_signal(&not_empty);
   empty = false;
   if ((int)_queue->size() == max_queue_size) {
      full = true;
   }
   printf("after push queue size : %ld\n", _queue->size());
   pthread_mutex_unlock(&queue_mutex);
}

void *mutex_task_queue::pop() {
   printf("in pop. waiting for mutex\n");
   pthread_mutex_lock(&queue_mutex);
   printf("getting mutex. empty is %d\n", empty);
   while (empty) { 
      pthread_cond_wait(&not_empty, &queue_mutex);
   }
   std::queue<void *>* _queue = get_queue();
   printf("queue size : %ld\n", _queue->size());
   void *qe_front = _queue->front();
   _queue->pop();
   if ((int)_queue->size() == 0) empty = true;
   if (full) pthread_cond_signal(&not_full);
   full = false;
   pthread_mutex_unlock(&queue_mutex);
   return qe_front;
}
