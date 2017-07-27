#include "log.h" 
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

LogUtil::LogUtil(std::string LogUtil_output, int LogUtil_level) {
   log_level = LogUtil_level;
   log_output = LogUtil_output.c_str();
   running = true;
   is_async = false;
   pthread_mutex_init(&async_lock, NULL);
   int ret = init();
   if (ret == -1) _exit(-1);
}

LogUtil::~LogUtil() {
   delete tq;
}

void LogUtil::stop() {
   running = false;
   void *ret;
   pthread_join(log_pid, &ret);
}

bool LogUtil::set_async(bool async) {
   pthread_mutex_lock(&async_lock);
   is_async = async;
   pthread_mutex_unlock(&async_lock);
   tq = new mutex_task_queue();
   int ret = pthread_create(&log_pid, NULL, log_from_queue, this);
   if (ret != 0) fprintf(stderr, "Use of async logging error. [set_async] : %s\n", strerror(errno));
   return ret == 0;
}

void *LogUtil::log_from_queue(void *arg) {
   LogUtil *logger_instance = (LogUtil *)arg;
   task_queue *tq = logger_instance->tq;
   if (!tq) {
      fprintf(stderr, "Use of async logging error. [log_from_queue] : task_queue is NULL\n");
      return NULL;
   } 
   /*
    * TODO:
    * Here should have a condition to let the thread stop
    */
   while (logger_instance->running /* stop condition*/) { 
      async_log_content *lc = (async_log_content *)tq->pop();
      write(logger_instance->output_fd, lc->log_content, LOG_LEN);
   }

   return NULL;
}

int LogUtil::init() {
   output_fd = open(log_output, O_RDWR | O_CREAT| O_APPEND, 0666);
   if (output_fd == -1) {
      fprintf(stderr, "log file init error. errormsg : %s\n", strerror(errno));
      return -1;
   }
   return 0;
}

void LogUtil::write_log(std::string log) {
   pthread_mutex_lock(&async_lock);
   if (is_async) {
      pthread_mutex_unlock(&async_lock);
      async_log_content *alc = new async_log_content();
      memcpy(alc->log_content, log.c_str(), LOG_LEN);
      tq->push(alc);
   } else {
      pthread_mutex_unlock(&async_lock); 
      write(output_fd, log.c_str(), log.length());
   }
}

void LogUtil::error(std::string _error) {
   if (log_level < ERROR) return ;
   if (!log_output) {
      fprintf(stderr, "output_file is NULL\n");
      return ;
   }
   write(output_fd, _error.c_str(), _error.length());
}

void LogUtil::debug(std::string _debug) {
   if (log_level < DEBUG) return ;
   char buf[128] = {0};
   sprintf(buf, "DEBUG : %s", _debug.c_str());
   write(output_fd, _debug.c_str(), _debug.length());
}

void LogUtil::info(std::string _info) { 
   if (log_level < INFO) return ;
   char buf[128] = {0};
   sprintf(buf, "INFO : %s", _info.c_str());
   write(output_fd, _info.c_str(), _info.length());
}

void LogUtil::debug(const char *fmt, ...) {
   int n;
   int size = 100;     /* Guess we need no more than 100 bytes */
   char *p, *np;
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
