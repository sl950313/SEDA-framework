#ifndef _JOB_QUEUE
#define _JOB_QUEUE

#include <pthread.h>
#include <queue>

#include "struct.h"

typedef void * (*worker_task)(void *);
struct queue_element {
   worker_task _cb;
   void *arg;
};

class task_queue {
public:
   task_queue();
   virtual ~task_queue() {};
   virtual void push(void *qe) {}
   virtual void *pop() {return NULL;}
   bool is_full();
   bool is_empty();
   std::queue<void *> *get_queue();
   bool empty, full; 
   int max_queue_size;

private:
   std::queue<void *> _queue;
};

class mutex_task_queue : public task_queue {
public:
   mutex_task_queue();
   void push(void *qe); 
   void *pop();

private:
   pthread_mutex_t queue_mutex;
   pthread_cond_t not_full, not_empty;
};
#endif //
