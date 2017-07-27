#include "receiver.h"
#include "log.h"
#include <zmq.h>
#include <assert.h>

using namespace std;

void receiver::run() { 
   void *context = zmq_ctx_new();
   void *subscriber = zmq_socket(context, ZMQ_SUB);

   int rc = zmq_connect(subscriber, res[0].c_str());
   assert(rc == 0);

   rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, NULL, 0);
   assert(rc == 0);
   char buf[256] = {
      0
   };
   while (running) { 
      memset(buf, 0, 256); 
      int nrecv = zmq_recv(subscriber, buf, 256, 0);
      if (nrecv == -1) {
         LogUtil::debug("receiver [run] error");
      }
      buf[255] = 0;
      IElement ie(buf);
      pthread_mutex_lock(&lock);
      elements.push(ie);
      pthread_mutex_unlock(&lock);
   }
}

/*
void receiver::setResources(vector<string> &res) {
   for (size_t i = 0; i < res.size(); ++i) {

   }
}
*/

IElement receiver::fetchOne() {
   pthread_mutex_lock(&lock);
   IElement ie = elements.front();
   elements.pop();
   pthread_mutex_unlock(&lock);
   return ie;
}
