#include "sender.h"
#include "log.h"
#include <assert.h>
#include <zmq.h>
#include <unistd.h>

sender::sender() {
   //init();
   running = true;
}

bool sender::sendMsg(IElement *ie) { 
   LogUtil::debug("sender : [sendMsg] ie : %s, %d", ie->getElement(), ie->length());
   int ret = zmq_send(publisher, ie->getElement(), ie->length(), ZMQ_DONTWAIT);
   if (ret == -1) { 
      if (errno == EAGAIN) {
         LogUtil::debug("sender : [sendMsg] the message cannot be sent at the moment");
         return false;
      }
   }
   LogUtil::debug("sender : [sendMsg] %s, %s", (ret == 0) ? "success" : "fail", (ret == 0) ? "" : strerror(errno)); 
   //assert(ret == 0);
   return true;
}

void sender::init() {
   LogUtil::debug("sender : [init] begin");
   void *context = zmq_ctx_new();
   assert(context != NULL);
   //int ret = zmq_ctx_set(context, ZMQ_MAX_SOCKETS, 1);
   //assert(ret == 0);
   publisher = zmq_socket(context, ZMQ_PUB);
   assert(publisher != NULL);
   int ret = zmq_bind(publisher, addr.c_str());
   LogUtil::debug("sender : [init] addr=%s", addr.c_str()); 
   if (ret != 0) {
      LogUtil::debug("sender : [init] zmq_bind fail. ret = %d, %s", zmq_errno(), zmq_strerror(zmq_errno()));
      sleep(3);
      exit(-1);
   }
   LogUtil::debug("sender : [init] success");
}

void *sender::run(void *arg) {
   sender *sd = (sender *)arg;
   while (sd->running) {
      break;
   }
   return sd;
}
