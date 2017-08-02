#include "stage_control.h"
#include "log.h"
#include <unistd.h>

void *stage_control::run(void *arg) {
   stage_control *sc = (stage_control *)arg;
   if (sc == NULL) {
      LogUtil::debug("stage_control : [run]. arg=NULL");
      return NULL;
   }
   LogUtil::debug("stage_control : %s", (sc->running == true) ? "running" : "stopping");
   while (sc->running) {
      //LogUtil::debug("stage_control running"); 
      LogUtil::debug("stage_control : worker_pool [worker_num=%d]", sc->wp->getRunnningWorkerNum());
      sleep(3);
   }
   return NULL;
}
