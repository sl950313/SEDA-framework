#ifndef _STAGE_HANDLER_H
#define _STAGE_HANDLER_H 
#include "struct.h"
#include "stage_queue.h"
#include "worker_pool.h"
#include "receiver.h"
#include <string>

class stage_handler {
public:
   stage_handler(receiver *, stage_queue *, worker_pool *wp);
   bool setHandler(Function f);
   void setStageQueue(stage_queue *sq);
   bool run();
   bool sendMsg(IElement *ie, std::string stage);

private:
   Function fun;
   receiver *rec;
   stage_queue *sq;
   worker_pool *wp;
   bool running;
};

#endif /* _STAGE_HANDLER_H */
