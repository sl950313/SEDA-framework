#include "../../log.h"
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {
   if (argc < 3) {
      fprintf(stderr, "logger_test [0 or 1] [len of log], 1 : async, 0 : sync\n");
      return 1;
   }
   if (atoi(argv[argc - 2]) == 1) LogUtil::set_async(true);
   LogUtil::init();
   struct timeval start, end;
   gettimeofday(&start, NULL);
   for (int i = 0; i < atoi(argv[argc - 1]); ++i) {
      LogUtil::debug("This is a logger debug test");
   } 
   for (int j = 0; j < 1000; ++j) {
      for (int i = 0; i < 1000; ++i) {
         float a = 1.000001;
         a *= 1.000001;
         a *= 1.000001;
         a *= 1.000001;
         a *= 1.000001;
         a *= 1.000001;
         a *= 1.000001;
      }
   }
   gettimeofday(&end, NULL);
   printf("logger stop command send...time : %ld, %ld\n", end.tv_sec - start.tv_sec, end.tv_usec - start.tv_usec);
   //LogUtil::stop();
   printf("logger stopped.\n");
   return 0;
}
