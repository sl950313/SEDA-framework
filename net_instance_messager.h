#ifndef _NET_INSTANCE_MESSAGER_H
#define _NET_INSTANCE_MESSAGER_H
#include "instance_messager.h"
#include <vector>
#include <string>
#include <map>

struct peer_addr {
   std::string ip;
   int port;
};

class net_instance_massager : public instance_messager {
public:
   net_instance_massager();
   ~net_instance_massager() {}

   int _write(std::string modulename, std::string buffer);
   int _read();

private:
   int port;
   int listenfd;
   std::vector<int> module_socks;
   std::map<std::string, int> modulename_sock;
};

#endif // _NET_INSTANCE_MESSAGER_H
