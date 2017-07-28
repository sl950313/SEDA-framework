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
   //LogUtil(std::string _log_output, int _log_level);
   static void error(const char *fmt, ...);
   static void debug(const char *fmt, ...);
   static void info(const char *fmt, ...);
   static bool set_async(bool async);
   static void stop();

public:
   static int init();
   static void make_message(const char *fmt, ...);
   static const char *log_output;
   static int output_fd;
   static int log_level;
   static void write_log(std::string log);
   static bool running;

   /*
    * for async logging
    */
   static pthread_mutex_t async_lock;
   static int is_async; 
   static pthread_t log_pid;
   static task_queue *tq;// = new mutex_task_queue();
   static void *log_from_queue(void *);

};

/*
pthread_mutex_t LogUtil::async_lock = PTHREAD_MUTEX_INITIALIZER;
int LogUtil::is_async = 1;
task_queue *LogUtil::tq = new mutex_task_queue();
bool LogUtil::running = true;
const char *LogUtil::log_output = "./log/server.log";
*/

#endif //
