#ifndef _STAGE_HANDLER_H
#define _STAGE_HANDLER_H 
#include "struct.h"
#include "stage_queue.h"
#include "worker_pool.h"
#include "receiver.h"
#include "sender.h"
#include <string>

class stage_handler {
public:
   stage_handler(receiver *, stage_queue *, worker_pool *wp);
   stage_handler(receiver *, stage_queue *, worker_pool *wp, sender *sd);
   bool setHandler(Function f);
   void setStageQueue(stage_queue *sq);
   static void *run(void *);
   static void *handlerFunc(void *);
   bool sendMsg(IElement *ie, std::string stage);
   Function getf() {return fun;}
   IElement &getElement() {return ie;}
   void setAddr(string &_publish_addr) {
      publish_addr = _publish_addr;
      sd->setDestination(_publish_addr);
      sd->init();
   }

private:
   Function fun;
   receiver *rec;
   stage_queue *sq;
   worker_pool *wp;
   sender *sd;
   bool running;

   IElement ie;
   string publish_addr;
};

#endif /* _STAGE_HANDLER_H */
