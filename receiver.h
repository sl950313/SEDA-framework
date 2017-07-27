#ifndef _RECEIVER_H
#define _RECEIVER_H 
#include "IElement.h"
#include <vector>
#include <string>
#include <queue>
#include <pthread.h>

using namespace std;

class receiver {
public:
   receiver() { 
      running = true;
      pthread_mutex_init(&lock, NULL);
   }
   void run();
   IElement fetchOne();
   void fetch(int count);

   void setResources(vector<string> &res) {
      this->res = res;
   }

   /*
    * Need to scale the interface for stage-control.
    */

private:

   std::vector<std::string> res;
   pthread_mutex_t lock;
   std::queue<IElement > elements;
   bool running;
};

#endif /* _RECEIVER_H */
