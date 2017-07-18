#ifndef _RECEIVER_H
#define _RECEIVER_H 
#include "stage_handler.h"

class receiver {
public:
   receiver(stage_handler *_sh) : sh(_sh){

   }
   void run();
   void fetchOne();
   void fetch(int count);

   /*
    * Need to scale the interface for stage-control.
    */

private:
   stage_handler *sh;
};

#endif /* _RECEIVER_H */
