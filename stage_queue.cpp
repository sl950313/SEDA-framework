#include "stage_queue.h"

IElement *stage_queue::qpop() {
   return (IElement *)pop();
}

void stage_queue::qpush(IElement *ie) {
   push(ie);
}
