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

bool stage_handler::run() {
   while (running) {
      //IElement ie = sq->qpop(); 
      IElement ie = rec->fetchOne();
      Function func(fun.getFunction(), ie.getElement());
      wp->run(func);
   }
   return true;
}
