#include "stage.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <vector>
#include <string>
#include <string.h>
#include "splaytree.h"
#include "utiltools.h"
#include <sys/time.h>

using namespace std;

void *lookUpRedis(void *arg) {
   LogUtil::debug("main [lookUpRedis] : %s", (char *)arg);
   return arg;
}

/*
 * search cache. 0x00 + [data]
 * insert into cache. 0x01 + [data]
 */
void *searchCache(void *arg) {
   LogUtil::debug("main [searchCache] : %s", (char *)arg);

   void *cache = NULL;
   cache = stage::getPersistData("cache");
   if (!cache) {
      stage::setPersistData("cache");
   }
   splay_tree *splay_tree_cache = (splay_tree *)cache;
   if (*(char *)arg == 1) {
      /*
       * search cache
       */
      test_msg tm;
      memcpy(&tm, (char *)arg + 1, sizeof(tm));
      LogUtil::debug("main [searchCache] : name=%s,passwd=%s", tm.name, tm.passwd);

      splay_tree *r = splaytree_splay(splay_tree_cache, utiltool::hash((char *)arg, strlen((char *)arg)));
      if (r) { 
         LogUtil::debug("main [searchCache] : cache hit");
         struct timeval time;
         gettimeofday(&time, NULL);
         tm.end_time = time.tv_sec % 1000 * 1000 + time.tv_usec / 1000;
         LogUtil::debug("main [success] : time cost=%ld", tm.end_time - tm.begin_time);
         return NULL;
      }
   } else {
      splaytree_insert(splay_tree_cache, utiltool::hash((char *)arg + 1, strlen((char *)arg + 1)), arg);
   }

   return (void *)((char *)arg + 1);
}

void *receiveAndResponse(void *arg) {
   LogUtil::debug("main [receiveAndResponse] : %s", (char *)arg);

   test_msg tm;
   memcpy(&tm, arg, sizeof(tm));
   LogUtil::debug("main [receiveAndResponse] : name=%s,passwd=%s\n", tm.name, tm.passwd);

   return arg;
}

void *print(void *arg) {
   //printf("main [print] : %s", (char *)arg);
   LogUtil::debug("main [print] : %s", (char *)arg);
   return arg;
}

void daemonize() {    
   int                  i, fd0, fd1, fd2;    
   pid_t                pid;    
   struct rlimit        rl;    
   struct sigaction    sa;    
   const char *cmd = "main";
   /*  
    *      * Clear file creation mask.  
    *          */    
   umask(0);    
   /*  
    *      * Get maximum number of file descriptors.  
    *          */    
   if (getrlimit(RLIMIT_NOFILE, &rl) < 0)    
      printf("%s: can't get file limit", cmd);    
   /*  
    *      * Become a session leader to lose controlling TTY.  
    *          */    
   if ((pid = fork()) < 0)    
      printf("%s: can'tork", cmd);    
   else if (pid != 0) /* parent */    
      exit(0);   

   setsid();    
   /*  
    *      * Ensure future opens won't allocate controlling TTYs.  
    *          */    
   sa.sa_handler = SIG_IGN;    
   sigemptyset(&sa.sa_mask);    
   sa.sa_flags = 0;    
   if (sigaction(SIGHUP, &sa, NULL) < 0)    
      printf("can't ignore SIGHUP");    
   if ((pid = fork()) < 0)    
      printf("%s: can't fork", cmd);    
   else if (pid != 0) /* parent */    
      exit(0);    
   /*  
    *      * Change the current working directory to the root so  
    *          * we won't prevent file systems from being unmounted.  
    *              */    
   if (chdir("/") < 0)    
      printf("can't change directory to /");    
   /*  
    *      * Close all open file descriptors.  
    *          */    
   if (rl.rlim_max == RLIM_INFINITY)    
      rl.rlim_max = 1024;    
   for (i = 0; i < (int)rl.rlim_max; i++)    
      close(i);    
   /*  
    *      * Attach file descriptors 0, 1, and 2 to /dev/null.  
    *          */    
   fd0 = open("/dev/null", O_RDWR);    
   fd1 = dup(0);    
   fd2 = dup(0);    
   /*  
    *      * Initialize the log file.  
    *          */    
   openlog(cmd, LOG_CONS, LOG_DAEMON);    
   if (fd0 != 0 || fd1 != 1 || fd2 != 2) {    
      syslog(LOG_ERR, "unexpected file descriptors %d %d %d",fd0, fd1, fd2);    
      exit(1);    
   }    
   syslog(LOG_DEBUG, "daem ok ");    
}

void runStage(vector<string> &res, string &des, void *(*handler)(void *), const char *stage_name) {
   LogUtil::init();
   LogUtil::debug("main.cpp : [main] config success");
   Config config(CONFIG_FILE);

   stage s(stage_name);
   s.setResources(res);
   s.setDestination(des);
   s.init(config);
   LogUtil::debug("main : [main] stage %s init success", stage_name);
   Function fun;
   fun.setFunction(handler);
   s.getHandler()->setHandler(fun);
   s.run();
}

int main(int argc, char **argv) { 
   //daemonize();

   vector<vector<string> > res;
   vector<string> _des;
   vector<string> resources;
   string des;
   resources.push_back("tcp://localhost:5666");
   //resources.push_back("tcp://localhost:5666");
   res.push_back(resources);
   resources.clear();
   resources.push_back("tcp://localhost:5667");
   res.push_back(resources);
   resources.clear();
   resources.push_back("tcp://localhost:5668");
   res.push_back(resources);
   _des.push_back("tcp://*:5667");
   _des.push_back("tcp://*:5668");
   _des.push_back("");

   pid_t pid;
   if ((pid = fork())) {
      /*
       * stage 1.
       */
      runStage(res[0], _des[0], receiveAndResponse, "receive_response");
   } else {
      if (pid < 0) {
         fprintf(stderr, "fork error\n");
         exit(-1);
      } else {
         if ((pid = fork())) {
            runStage(res[1], _des[1], searchCache, "search_cache");
         } else {
            if (pid < 0) {
               fprintf(stderr, "fork error\n");
               exit(-1);
            }
            runStage(res[2], _des[2], lookUpRedis, "lookup_redis");
         }
      }
   }

   return 0;
}
