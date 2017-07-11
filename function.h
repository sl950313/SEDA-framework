#ifndef _FUNCTION_H
#define _FUNCTION_H 
#include "struct.h"

class Function {
public:
   Function(function fun, void *_arg) : f(fun), arg(_arg) {}
   function getFunction() {
      return f;
   }

   void *getArg() {
      return arg;
   }

private:
   function f;
   void *arg;

};

#endif /* _FUNCTION_H */
