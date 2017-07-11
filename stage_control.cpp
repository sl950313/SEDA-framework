#include "stage_control.h"
#include "log.h"
#include <unistd.h>

void stage_control::run() {
   while (running) {
      LogUtil::debug("stage_control running");
      sleep(3);
   }
}
