#ifndef _STAGE_CONTROL_H
#define _STAGE_CONTROL_H 
#include "worker_pool.h"
#include "stage_queue.h"

class stage_control {
public:
   stage_control(stage_queue *_sq, worker_pool *_wp) : sq(_sq), wp(_wp){ 
   }
   static void *run(void *);
   bool getRunning() {
      return running;
   }

private:
   bool running;
   stage_queue *sq;
   worker_pool *wp;
};

#endif /* _STAGE_CONTROL_H */
