#ifndef _LOOP_H
#define _LOOP_H 
#include "socket.h"
#include "stage_queue.h"

class Loop {
public:
   Loop(Socket *_s, stage_queue *_sq) : s(_s), sq(_sq) { }
   void loop();

private:
   void *accept_conn();
   void *read_conn(void *conn);
   void *write_conn(void *conn);
   Socket *s;

   stage_queue *sq;
};

#endif /* _LOOP_H */
