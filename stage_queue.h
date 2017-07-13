#ifndef _STAGE_QUEUE_H
#define _STAGE_QUEUE_H 
#include "IElement.h"
#include "job_queue.h"

class stage_queue : public mutex_task_queue{
public:
   IElement *qpop();
   void qpush(IElement *ie);
};

#endif /*_STAGE_QUEUE_H*/
