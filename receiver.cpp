#include "receiver.h"
#include "log.h"
#include <zmq.h>
#include <assert.h>

using namespace std;

void *receiver::run(void *arg) { 
   receiver *rc = (receiver *)arg;
   void *context = zmq_ctx_new();
   void *subscriber = zmq_socket(context, ZMQ_SUB);

   LogUtil::debug("here ? ");
   LogUtil::debug("res : %s", rc->res[0].c_str());
   int r = zmq_connect(subscriber, rc->res[0].c_str());
   if (r != 0) {
      LogUtil::debug("receiver | run | zmq_connect error.");
      rc->running = false;
      return NULL;
   }
   //assert(rc == 0);
   LogUtil::debug("receiver : [run], subscriber : %s", rc->res[0].c_str());

   r = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, NULL, 0);
   //assert(rc == 0);
   LogUtil::debug("receiver : [run] zmq finish!");
   char buf[256] = {
      0
   };
   while (rc->running) { 
      memset(buf, 0, 256); 
      int nrecv = zmq_recv(subscriber, buf, 256, 0);
      if (nrecv == -1) {
         LogUtil::debug("receiver [run] error");
      }
      buf[255] = 0;
      IElement ie(buf);
      pthread_mutex_lock(&rc->lock);
      rc->elements.push(ie);
      pthread_mutex_unlock(&rc->lock);
   }
   return rc;
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
