#include "event_core.h"



void event_core::run() {
   l->loop();
}

bool event_core::init() {
   s = new Socket(config.getListenPort()); 
   l = new Loop(s);
   return true;
}
