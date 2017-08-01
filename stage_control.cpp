#include "stage_control.h"
#include "log.h"
#include <unistd.h>

void *stage_control::run(void *arg) {
   stage_control *sc = (stage_control *)arg;
   while (sc->running) {
      LogUtil::debug("stage_control running");
      sleep(3);
   }
   return NULL;
}
