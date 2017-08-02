#ifndef _FUNCTION_H
#define _FUNCTION_H 
#include "struct.h"
#include <stdlib.h>

class Function {
public:
   Function() {
      Function(NULL, NULL);
   }
   Function(function fun, void *_arg) : f(fun), arg(_arg) {}
   function getFunction() {
      return f;
   }

   void *getArg() {
      return arg;
   }

   void setFunction(function _f) {
      f = _f;
   }

private:
   function f;
   void *arg;

};

#endif /* _FUNCTION_H */
