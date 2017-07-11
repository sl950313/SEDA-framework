#include "stage_handler.h"
#include "function.h"

stage_handler::stage_handler(stage_queue *_sq, worker_pool *_wp) : sq(_sq), wp(_wp){ 
}

bool stage_handler::setHandler(function f) {
   fun = f;
   return true;
}

void stage_handler::setStageQueue(stage_queue *sq) {
   this->sq = sq;
}

bool stage_handler::run() {
   while (running) {
      IElement ie = sq->pop(); 
      Function func(fun, ie.getElement());
      wp->run(func);
   }
   return true;
}