#ifndef _LOOP_H
#define _LOOP_H 
#include "socket.h"

class Loop {
public:
   Loop(Socket *_s) : s(_s) { }
   void accept_conn();
   void loop();

private:
   Socket *s;
};

#endif /* _LOOP_H */
