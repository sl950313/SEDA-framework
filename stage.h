#ifndef _STAGE_H
#define _STAGE_H
#include <string> 
#include "stage_queue.h"
#include "stage_handler.h"
#include "worker_pool.h"
#include "stage_control.h"
#include "event_core.h"
#include "log.h"

using namespace std;

class stage { 
public:
   stage();
   stage(string stage_name);
   string get_stage_name();
   bool run();
   bool setHandler(stage_handler *sh);

private:
   bool init();
   string stage_name;

   stage_queue *sq;
   stage_handler *sh;
   worker_pool *wp;
   stage_control *sc;
   event_core *ec;
};

#endif /* _STAGE_H */
