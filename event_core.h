#ifndef _EVENT_CORE_H
#define _EVENT_CORE_H 
#include "config.h"
#include "socket.h"
#include "loop.h"

class event_core {
public:
   event_core(Config &_config) : config(_config) { 
      init();
   }
   void run();
   bool init();
   
private:
   Socket *s;
   Loop *l;
   Config config;
};

#endif /*_EVENT_CORE_H*/
