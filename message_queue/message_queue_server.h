#ifndef _message_queue_server_H
#define _message_queue_server_H
#include "worker_pool.h"
#include "log.h"
#include "mq_conn.h"
#include "job_queue.h"
#include <string>
#include <pthread.h>

enum MODE {
   PUBLISH = 0x1,
   SUBSCRIBE = 0x1  << 1
};

/*
 * The message is like this :
 * 
 * 通信消息格式:
 *    0x1 ... 0x1 : 由PUBLISHER发送过来,并将发送给其他SUBSCRIBERS的消息.
 *    0x2 ... 0x2 : PUBLICSHER发送过来的注册消息.
 *    0x4 ... 0x4 : SUBSCRIBER发送过来的注册消息.
 */

class message_queue_server {
public:
   message_queue_server();
   ~message_queue_server();
   //message_queue_server(int _mode);

   int read_config(std::string config);
   int init_mq();
   bool start();

private:
   std::map<std::string, std::string> relations;
   std::map<std::string, int> fd_addrs; 
   void convert_addr_in_to_str(struct sockaddr_in *addr, std::string &dst);

   int send(std::string msg);
   int recv();
   //int bind(std::string addr);
   int connect(std::string addr);
   int set_mq_mode(int _mode);

   int mode;
   int epfd;
   pthread_mutex_t epfd_mutex;
   //int log_fd;
   _log *log;
   int init_poller();

   worker_pool *wp;
   task_queue *tq;

   pthread_t poller_pid;
   void *poller(void *);
   int listenfd;
   static void *accept_conn(void *);
   static void *recv_msg(void *);
};

#endif // _message_queue_server_H
