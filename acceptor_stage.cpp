#include "acceptor.h"
#include "worker_pool.h"

int main() {
   LogUtil::init();
   LogUtil::debug("acceptor stage begin initialized"); 
   acceptor *ac = new acceptor();
   ac->start();
}
