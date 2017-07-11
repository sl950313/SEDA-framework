#ifndef _STAGE_QUEUE_H
#define _STAGE_QUEUE_H 
#include "IElement.h"

class stage_queue {
public:
   IElement pop();
   void push(IElement ie);
};

#endif /*_STAGE_QUEUE_H*/
