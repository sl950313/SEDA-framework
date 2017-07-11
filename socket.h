#ifndef _SOCKET_H
#define _SOCKET_H 
#include <string>

class Socket {
public:
   Socket(std::string ip, int port);
   Socket(int listen_port);
   void loop();
   bool connect(std::string ip, int port);
   bool write();
   bool read();
   int getListenfd() {
      return listenfd;
   }

private:
   bool listen();
   bool initServer();

   int listen_port;
   int listenfd;
   
   /*
    * For epoll 
    */
   int epfd;
};

#endif /* _SOCKET_H */
