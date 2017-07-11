#ifndef _LOG_H
#define _LOG_H

#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdarg.h>
#include "job_queue.h"
#include "marcos.h"

enum {
   ERROR = 1,
   INFO = 2,
   DEBUG = 3
};

struct async_log_content  {
   char log_content[LOG_LEN];
   unsigned int len_content;
   async_log_content() {
      memset(log_content, 0, LOG_LEN);
      len_content = 0;
   }
};

class LogUtil {
public:
   LogUtil();
   ~LogUtil();
   LogUtil(std::string _log_output, int _log_level);
   void error(std::string _error);
   void debug(std::string _debug);
   void info(std::string _info);
   static void error(const char *fmt, ...);
   static void debug(const char *fmt, ...);
   static void info(const char *fmt, ...);
   bool set_async(bool async);
   void stop();

private:
   int init();
   void make_message(const char *fmt, ...);
   const char *log_output;
   int output_fd;
   int log_level;
   void write_log(std::string log);
   bool running;

   /*
    * for async logging
    */
   pthread_mutex_t async_lock;
   int is_async; 
   pthread_t log_pid;
   task_queue *tq;
   static void *log_from_queue(void *);

};

#endif //
