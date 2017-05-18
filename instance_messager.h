#ifndef _INSTANCE_MESSAGER_H
#define _INSTANCE_MESSAGER_H
#include <string>

class instance_messager {
public:
   instance_messager() {}
   virtual ~instance_messager() {}

   virtual int _write(std::string modulename, std::string buffer) = 0;
   virtual int _read() = 0;
};

#endif //_INSTANCE_MESSAGER_H
