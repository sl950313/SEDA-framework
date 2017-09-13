#include "log.h" 
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

pthread_mutex_t LogUtil::async_lock = PTHREAD_MUTEX_INITIALIZER;
int LogUtil::is_async = 1;
task_queue *LogUtil::tq = new mutex_task_queue();
bool LogUtil::running = true;
const char *LogUtil::log_output = "./log/server";
int LogUtil::output_fd = 0;
int LogUtil::log_level = DEBUG;
pthread_t LogUtil::log_pid = 0;
/*
LogUtil::LogUtil(std::string LogUtil_output, int LogUtil_level) {
   LogUtil::log_level = LogUtil_level;
   LogUtil::log_output = LogUtil_output.c_str();
   running = true;
   LogUtil::is_async = false;
   pthread_mutex_init(&LogUtil::async_lock, NULL);
   int ret = init();
   if (ret == -1) _exit(-1);
}
*/

void LogUtil::stop() {
   running = false;
   void *ret;
   if (is_async) pthread_join(log_pid, &ret);
   delete tq;
}

bool LogUtil::set_async(bool async) {
   pthread_mutex_lock(&async_lock);
   is_async = async;
   pthread_mutex_unlock(&async_lock);
   return true;
}

void *LogUtil::log_from_queue(void *) {
   task_queue *_tq = tq;
   if (!_tq) {
      fprintf(stderr, "Use of async logging error. [log_from_queue] : task_queue is NULL\n");
      return NULL;
   } 
   /*
    * TODO:
    * Here should have a condition to let the thread stop
    */
   while (running /* stop condition*/) { 
      async_log_content *lc = (async_log_content *)_tq->pop();
      write(output_fd, lc->log_content, LOG_LEN);
   }

   return NULL;
}

int LogUtil::init() {
   char out[128] = {0};
   sprintf(out, "%s_%d.log", log_output, getpid());
   output_fd = open(out, O_RDWR | O_CREAT| O_APPEND, 0666);
   if (output_fd == -1) {
      fprintf(stderr, "log file init error. errormsg : %s\n", strerror(errno));
      return -1;
   }
   if (is_async) {
      int ret = pthread_create(&log_pid, NULL, log_from_queue, NULL);
      if (ret != 0) fprintf(stderr, "Use of async logging error. [set_async] : %s\n", strerror(errno));
      return ret == 0;
   }
   return 0;
}

void LogUtil::write_log(std::string log) {
   pthread_mutex_lock(&async_lock);
   if (is_async) {
      pthread_mutex_unlock(&async_lock);
      async_log_content *alc = new async_log_content();
      memcpy(alc->log_content, log.c_str(), log.length());
      tq->push(alc);
   } else {
      pthread_mutex_unlock(&async_lock); 
      write(output_fd, log.c_str(), log.length());
   }
}

void LogUtil::debug(const char *fmt, ...) {
   int n;
   int size = 100;     /* Guess we need no more than 100 bytes */
   char *p = NULL, *np = NULL;
   va_list ap;

   if ((p = (char *)malloc(size)) == NULL)
      return ;

   while (1) {

      /* Try to print in the allocated space */

      va_start(ap, fmt);
      n = vsnprintf(p, size, fmt, ap);
      va_end(ap);

      /* Check error code */

      if (n < 0)
         return ;

      /* If that worked, return the string */

      if (n < size) {
         char buf[128] = {0};
         sprintf(buf, "DEBUG : %s\n", p);
         write_log((std::string)buf);
         
         //write(output_fd, buf, strlen(buf));
         return ;
         //return p;
      }

      /* Else try again with more space */

      size = n + 1;       /* Precisely what is needed */

      if ((np = (char *)realloc (p, size)) == NULL) {
         free(p);
         return ;
      } else {
         p = np;
      }
   }
}
