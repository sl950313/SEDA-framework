#include "stage_handler.h"
#include "log.h"

stage_handler::stage_handler(receiver *_rec, stage_queue *_sq, worker_pool *_wp) { 
   rec = _rec;
   sq = _sq;
   wp = _wp;
   running = true;
   sd = new sender();
   //fun = NULL;
}

stage_handler::stage_handler(receiver *_rec, stage_queue *_sq, worker_pool *_wp, sender *_sd) {
   stage_handler(_rec, _sq, _wp);
   sd = _sd;
}

bool stage_handler::setHandler(Function f) {
   fun = f;
   return true;
}

void stage_handler::setStageQueue(stage_queue *sq) {
   this->sq = sq;
}

void *stage_handler::handlerFunc(void *arg) {
   stage_handler *sh = (stage_handler *)arg;
   void *res_msg = sh->getf().getFunction()(sh->getElement().getElement());
   if (!res_msg) 
      return NULL;
   IElement ie((char *)res_msg, strlen((char *)res_msg));
   sh->sd->sendMsg(&ie);
   return NULL;
}

void *stage_handler::run(void *arg) {
   stage_handler *sh = (stage_handler *)arg;
   if (!sh) {
      LogUtil::debug("stage_handler : [run]. error. arg=NULL");
      return NULL;
   }
   if (sh->getf().getFunction() == NULL) {
      LogUtil::debug("stage_handler : [run]. the setHandler should be involved first.");
      return NULL;
   }
   LogUtil::debug("stage_handler [run]. running");
   while (sh->running) {
      //IElement ie = sq->qpop(); 
      sh->ie = sh->rec->fetchOne();
      //Function func(sh->fun.getFunction(), sh->ie.getElement());
      Function func(handlerFunc, sh);
      sh->wp->run(func);
   }
   return sh;
}
