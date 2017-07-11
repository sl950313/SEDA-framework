#ifndef _CONFIG_H
#define _CONFIG_H 
#include "marcos.h"

class Config {
private:
   int listen_port;

public:
   Config(const char *config_file);
   int getListenPort() {
      return listen_port;
   }
};

#endif /* _CONFIG_H */
