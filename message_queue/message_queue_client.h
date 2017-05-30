#ifndef _MQ_CLIENT_H
#define _MQ_CLIENT_H
#include <string>
#include <pthread.h>
#include <signal.h>

typedef void * (*callback)(void *);
class mq_client {
public:
   mq_client(std::string ip_port);
   ~mq_client();
   int connect();
   int register_mq(int role, char *publisher_name); // 0 for PUBLISH, 1 for SUBSCRIBE.
   int unregister_mq(int role, char *publisher_name);
   int send(std::string msg);
   void loop(callback _cb);

private:
   std::string server_ip_port;
   pthread_t pid;
   callback _cb;
   int fd;
   static void *run(void *);
   static void sig_handle(int sig, siginfo_t *st, void *ret);
   void wait();

   int get_ip_port(std::string &server_ip_port, std::string &server_ip, int &server_port);
};

#endif //
