#include <unistd.h>
#include "net_instance_messager.h"

net_instance_massager::net_instance_massager() {
}

int net_instance_massager::_write(std::string modulename, std::string buffer) {
   std::map<std::string, int>::iterator it = modulename_sock.find(modulename);
   if (it == modulename_sock.end()) {
      printf("the module name : %s is not in our system. serious fault\n", modulename.c_str());
      return -1;
   }

   int peer_sock = it->second;
   int nwrite = write(peer_sock, buffer.c_str(), buffer.length());
   if (nwrite < (int)buffer.length()) {
      printf("Code here should be change, because the write system call may fail\n");
   }
   return nwrite;
}

int net_instance_massager::_read() {

   return 0;
}

