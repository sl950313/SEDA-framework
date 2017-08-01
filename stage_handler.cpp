#include "stage_handler.h"

stage_handler::stage_handler(receiver *_rec, stage_queue *_sq, worker_pool *_wp) { 
   rec = _rec;
   sq = _sq;
   wp = _wp;
   running = true;
}

bool stage_handler::setHandler(Function f) {
   fun = f;
   return true;
}

void stage_handler::setStageQueue(stage_queue *sq) {
   this->sq = sq;
}

void *stage_handler::run(void *arg) {
   stage_handler *sh = (stage_handler *)arg;
   while (sh->running) {
      //IElement ie = sq->qpop(); 
      IElement ie = sh->rec->fetchOne();
      Function func(sh->fun.getFunction(), ie.getElement());
      sh->wp->run(func);
   }
   return sh;
}
