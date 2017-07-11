#ifndef _CONFIG_H
#define _CONFIG_H 

class Config {
private:
   int listen_port;

public:
   int getListenPort() {
      return listen_port;
   }
};

#endif /* _CONFIG_H */
