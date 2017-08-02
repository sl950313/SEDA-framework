#include "receiver.h"
#include "log.h"
#include <zmq.h>
#include <assert.h>

using namespace std;

void *receiver::run(void *arg) { 
   receiver *rc = (receiver *)arg;
   void *context = zmq_ctx_new();
   assert(context != NULL);
   //int r = zmq_ctx_set(context, ZMQ_MAX_SOCKETS, 1);
   //assert(ret == 0);
   void *subscriber = zmq_socket(context, ZMQ_SUB);
   assert(subscriber != NULL);

   int r = zmq_connect(subscriber, rc->res[0].c_str());
   if (r != 0) {
      LogUtil::debug("receiver | run | zmq_connect error.");
      rc->running = false;
      return NULL;
   }
   //assert(rc == 0);
   LogUtil::debug("receiver : [run], subscriber : %s", rc->res[0].c_str());

   r = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);
   if (r != 0) {
      LogUtil::debug("receiver | run | zmq_setsockopt error.");
      rc->running = false;
      return NULL;
   }
   //assert(rc == 0);
   LogUtil::debug("receiver : [run] zmq finish!");
   char buf[256] = {
      0
   };
   while (rc->running) { 
      memset(buf, 0, 256); 
      LogUtil::debug("receiver : [run] recving...");
      int nrecv = zmq_recv(subscriber, buf, 256, 0);
      if (nrecv == -1) {
         LogUtil::debug("receiver [run] recving error");
         continue;
      }
      LogUtil::debug("receiver : [run] already recv");
      buf[255] = 0;
      IElement ie(buf);
      pthread_mutex_lock(&rc->lock);
      if (rc->elements.size() > MAX_QUEUE_SIZE) {
         LogUtil::debug("receiver [run], the rc->queue is less");
      }
      bool is_signal = false;
      if (rc->elements.empty()) { 
         is_signal = true;
      }
      rc->elements.push(ie);
      pthread_mutex_unlock(&rc->lock);
      if (is_signal)
         pthread_cond_signal(&rc->empty_queue);
         //pthread_cond_broadcast(&rc->empty_queue); 
   }
   return rc;
}

IElement receiver::fetchOne() {
   pthread_mutex_lock(&lock);
   while (elements.empty()) {
      LogUtil::debug("receiver : [fetchOne] waiting...");
      pthread_cond_wait(&empty_queue, &lock);
   } 
   IElement ie = elements.front();
   elements.pop();
   pthread_mutex_unlock(&lock);
   LogUtil::debug("receiver : [fetchOne] fetchOne success. %s", ie.getElement());
   return ie;
}
