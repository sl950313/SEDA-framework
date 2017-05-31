#include "message_queue_server.h"
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

message_queue_server::message_queue_server() {
   log = new _log(MQ_LOG_PATH, DEBUG);
   wp = new thread_worker_pool();
   tq = new mutex_task_queue();
   wp->set_task_queue(tq);
}

message_queue_server::~message_queue_server() {
   delete log;
   delete wp;
   delete tq;
}

int message_queue_server::init_mq() { 
   //pthread_create(&poller_pid, NULL, poller, this);
   log->debug("message_queue_server : [init_mq]\n");
   assert((init_poller() == 0));
   wp->start();
   poller(NULL);
   return 0;
}

int message_queue_server::init_poller() {
   log->debug("message_queue_server : [init_poller]\n");
   int ret = 0;
   listenfd = socket(AF_INET, SOCK_STREAM, 0);
   struct sockaddr_in actual_addr;
   actual_addr.sin_family = AF_INET;
   actual_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   actual_addr.sin_port = htons(MQ_PORT);

   // set the listenfd socket to be nonblock.
   fcntl(listenfd, F_SETFL, SOCK_NONBLOCK);

   //set listenfd reused.
   int option = 1;
   setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
   struct sockaddr *addr = (struct sockaddr *)&actual_addr;
   ret = bind(listenfd, addr, sizeof(actual_addr));
   assert(ret != -1);
   listen(listenfd, 50);
   return 0;
}

int message_queue_server::send(std::string msg) {
   return 0;
}

void *message_queue_server::poller(void *arg) {
   epfd = epoll_create1(0); // may change to EPOLL_CLOEXEC.
   assert((epfd != -1)); 

   struct epoll_event event;
   memset(&event, 0, sizeof(event));
   event.events = EPOLLIN | EPOLLET;
   event.data.fd = listenfd;
   epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);
   struct epoll_event events[MAXEVENTS];

   
   printf("message_queue_server : [poller] start poller\n");
   log->debug("message_queue_server : [poller] start poller\n");
   log->_debug("message_queue_server : [poller] start poller");
   for (;;) {
      int rd_fds = epoll_wait(epfd, events, MAXEVENTS, 1000);
      for (int i = 0; i < rd_fds; ++i) {
         if (events[i].data.fd == listenfd) {
            printf("here\n");
            queue_element *qe = new queue_element();
            qe->_cb = accept_conn;
            qe->arg = this;
            tq->push(qe);
            //accept_conn();
         } else {
            if (events[i].events & EPOLLIN) {
               // TODO
               // recv msg and choose subscribers to send.
               queue_element *qe = new queue_element();
               qe->_cb = recv_msg;
               qe->arg = (void *)events[i].data.ptr; 
               tq->push(qe);
               //recv_msg();
            } else {
               if (events[i].events & EPOLLOUT) {
                  // TODO.
                  // we may not use this.
               }
            }
         }
      }
   }
   return NULL;
}

void *message_queue_server::accept_conn(void *arg) {
   message_queue_server *mqs = (message_queue_server *)arg;
   mqs->log->_debug("message_queue_server : [accept_conn]\n");
   printf("in accept_conn\n");

   struct sockaddr_in actual_addr;
   int len = sizeof(struct sockaddr);
   int peer_sock = -1;
   std::string addr_str;
   struct epoll_event event;
   while ((peer_sock = accept(mqs->listenfd, (sockaddr *)&actual_addr, (socklen_t *)&len)) > 0) {
      mq_conn *mc = new mq_conn();
      mc->fd = peer_sock;
      mc->epfd = mqs->epfd;
      mc->mqs = mqs;
      mqs->convert_addr_in_to_str(&actual_addr, addr_str);
      mc->addr = addr_str;
      mqs->fd_addrs.insert(std::pair<std::string, int>(addr_str, peer_sock));
      fcntl(peer_sock, F_SETFL, SOCK_NONBLOCK);
      memset(&event, 0, sizeof(event));
      event.events = EPOLLIN | EPOLLET;
      event.data.ptr = mc;
      epoll_ctl(mqs->epfd, EPOLL_CTL_ADD, peer_sock, &event); 
   }

   if (peer_sock == -1) {
      if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR)
         perror("accept");
   }
   return NULL;
}

void *message_queue_server::recv_msg(void *arg) {
   //int peer_sock = (int)(long)arg;
   mq_conn *mc = (mq_conn *)arg;
   int nread = -1;
   char _buf[BUFSIZE] = {0};
   printf("in recv msg\n");
   while ((nread = read(mc->fd, _buf, BUFSIZE - 1)) > 0) { 
      /*
      if (!mc->mqs->check_msg_crc(_buf, nread)) {
         mc->mqs->log->debug("the msg of crc is not right\n");
         break;
      }
      */
      switch (_buf[0]) {
      case 0x1: 
         if (mc->mqs->check_msg_crc(_buf, nread)) { 
            buffer *recved_msg = mq_def::encode_mq_msg(_buf);
            std::vector<std::string> all_subs = mc->mqs->relations[mc->addr];
            mc->mqs->log->_debug("recv msg from publisher : %s\n", mq_def::decode_mq_msg(recved_msg).c_str());
            for (size_t i = 0; i < all_subs.size(); ++i) {
               std::map<std::string, int>::iterator it = mc->mqs->fd_addrs.find(all_subs[i]);
               if (it == mc->mqs->fd_addrs.end()) {
                  // insert into cache of the subsciber.
                  mc->mqs->cached_msg[all_subs[i]].push_back(mq_def::decode_mq_msg(recved_msg));
               } else { 
                  // mv out all msg in cache of the subscriber.
                  vector<string> msgs = mc->mqs->cached_msg[all_subs[i]];
                  for (size_t i = 0; i < msgs.size(); ++i) {
                     write(mc->fd, msgs[i].c_str(), msgs[i].length());
                  }
                  write(mc->fd, mq_def::decode_mq_msg(recved_msg).c_str(), nread);
               }
               //int sub_fd = mc->mqs->fd_addrs[all_subs[i]]; 
            }
            delete recved_msg;
         } else {
            mc->mqs->log->debug("the msg of crc is not right\n");
         }
         break; 
      case 0x2: 
         mc->mqs->log->_debug("recv publish register msg. %s", mc->addr.c_str());
         mc->mqs->publishers.push_back(mc->addr);
         mc->mqs->fd_addrs.insert(std::pair<std::string, int>(mc->addr, mc->fd)); 
         break;
      case 0x4: {
                   mc->mqs->fd_addrs.insert(std::pair<std::string, int>(mc->addr, mc->fd));
                   char publisher_name[128] = {0};
                   memcpy(publisher_name, _buf + 1, nread - 2); 
                   mc->mqs->log->_debug("recv subscibe register msg : %s", publisher_name);
                   map<string, int>::iterator it = mc->mqs->fd_addrs.find(publisher_name);
                   if (it == mc->mqs->fd_addrs.end()) {
                      mc->mqs->log->_debug("subscribe error. %s", publisher_name);
                   } else {
                      mc->mqs->log->_debug("subscribe success. %s", publisher_name); 
                      mc->mqs->relations[publisher_name].push_back(mc->addr);
                   }
                   //string ip, port; 
                   //mc->mqs->get_ip_port_from_publisher_name(ip, port, publisher_name); 
                   break;
                }
      case 0x8:
                mc->mqs->log->_debug("recv publisher unregister msg");
                mc->mqs->fd_addrs.erase(mc->mqs->fd_addrs.find(mc->addr));
                mc->mqs->relations.erase(mc->mqs->relations.find(mc->addr));
                break;
      case 0x16:
                {
                   // TODO.
                   char publisher_name[128] = {0};
                   memcpy(publisher_name, _buf + 1, nread - 2); 
                   mc->mqs->log->_debug("recv subsciber unregister msg");
                   //mc->mqs->relations[publisher_name].remove(mc->mqs->relations[publisher_name].find(mc->addr));
                }
                break;
      default:
                break;
      }
      // Here should be changed.
   }

   if (nread == 0) {
      printf("%lu, %d read 0. client closed\n", pthread_self(), mc->fd);
      struct epoll_event event;
      memset(&event, 0, sizeof(event));
      int ret = epoll_ctl(mc->epfd, EPOLL_CTL_DEL, mc->fd, &event);
      if (ret == -1) {
         //perror("epoll_ctl : read_conn");
         printf("epoll_ctl : EPOLL_CTL_DEL : %s\n", strerror(errno));
      }
      close(mc->fd);
      delete mc;
   }
   return NULL;
}

bool message_queue_server::check_msg_crc(char *buf, int n) {
   char crc_sum = 0x0;
   for (int i = 0; i < n - 1; ++i) { crc_sum += buf[i]; }
   //log->_debug("crc_sum : %d <> %d", crc_sum, buf[n - 1]);
   return !(buf[n - 1] ^ crc_sum);
}

void message_queue_server::convert_addr_in_to_str(struct sockaddr_in *addr, std::string &dst) {
   char _addr[128] = {0}, _dst[128] = {0};
   //memset(_addr, 0, 128);
   //inet_ntop(AF_INET, addr, _addr, sizeof(_addr));
   //sprintf(_dst, "%s:%d", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
   sprintf(_dst, "%s", inet_ntoa(addr->sin_addr));
   dst = _dst;
   log->_debug("dst : %s", dst.c_str());
   //dst = _addr + ":" + std::to_string(ntohs(addr->sin_port));
}

void message_queue_server::get_ip_port_from_publisher_name(string &ip, string &port, char *publisher_name) {
   char *p = publisher_name;
   while (*p != ':') {++p;}
   *p = '\0';
   ip = publisher_name;
}
