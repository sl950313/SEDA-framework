#ifndef _SLAB_SYSTEM_H
#define _SLAB_SYSTEM_H
#include <map>
#include <string>

class slab {
public:
   bool alloc1();
   bool free();

private:
   unsigned int num;
};
typedef std::map<std::string, slab *> slab_system;

#endif //
