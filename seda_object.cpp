#include "seda_object.h"
#include "log.h"

sedaObject::sedaObject() : refcount(0){
}

sedaObject::~sedaObject() {
}

void sedaObject::incrRefCount() {
   ++refcount;
}

void sedaObject::decrRefCount() {
   if (refcount <= 0) {
      LogUtil::debug("decrRefCount against refcount <= 0");
      return ;
   } 
   if (refcount == 1) { 
      delete this;
   } else {
      --refcount;
   }
}

void sedaObject::resetRefcount() {
   refcount = 0;
}
