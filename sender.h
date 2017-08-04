#ifndef _SENDER_H
#define _SENDER_H
#include "IElement.h"
#include <string>
#include <vector>

using namespace std;

class sender {
public:
   sender();
   bool sendMsg(IElement *ie);
   static void *run(void *);
   void setDestination(string &des) {
      this->addr = des;
   }
   bool getRunningState() {return running;}
   void init();

private:
   bool running;
   void *publisher;
   std::string addr;
};

#endif /*_SENDER_H */
