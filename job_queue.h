#ifndef _JOB_QUEUE
#define _JOB_QUEUE

#include <pthread.h>
#include <queue>

#include "struct.h"

struct queue_element {
   int fd;
   connect_status _status;
   queue_element() : fd(-1), _status(CONNECT){}
};

typedef void * (*worker_task)(void *);
class task_queue {
public:
   task_queue();
   virtual void push(queue_element *qe) {}
   virtual queue_element *pop() {return NULL;}
   bool is_full();
   bool is_empty();
   std::queue<queue_element *> *get_queue();
   bool empty, full; 
   int max_queue_size;

private:
   std::queue<queue_element *> _queue;
};

class mutex_task_queue : public task_queue {
public:
   mutex_task_queue();
   void push(queue_element *qe); 
   queue_element *pop();

private:
   pthread_mutex_t queue_mutex;
   pthread_cond_t not_full, not_empty;
};
#endif //
