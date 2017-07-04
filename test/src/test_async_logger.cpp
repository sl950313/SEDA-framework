#include "../../log.h"
#include <stdio.h>

int main(int argc, char **argv) {
   _log *logger = new _log("./log_test.log", DEBUG);
   //logger->set_async(true);
   for (int i = 0; i < 128; ++i) {
      logger->_debug("This is a logger debug test");
   }

   printf("logger stop command send...\n");
   logger->stop();
   printf("logger stopped.\n");
   delete logger;
   return 0;
}
