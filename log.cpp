#include "log.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

_log::_log(std::string _log_output, int _log_level) {
   log_level = _log_level;
   log_output = _log_output.c_str();
   int ret = init();
   if (ret == -1) _exit(-1);
}

int _log::init() {
   output_fd = open(log_output, O_RDWR | O_CREAT| O_APPEND, 0666);
   if (output_fd == -1) {
      fprintf(stderr, "log file init error. errormsg : %s\n", strerror(errno));
      return -1;
   }
   return 0;
}

void _log::error(std::string _error) {
   if (log_level < ERROR) return ;
   if (!log_output) {
      fprintf(stderr, "output_file is NULL\n");
      return ;
   }
   write(output_fd, _error.c_str(), _error.length());
}

void _log::debug(std::string _debug) {
   if (log_level < DEBUG) return ;
   char buf[128] = {0};
   sprintf(buf, "DEBUG : %s", _debug.c_str());
   write(output_fd, _debug.c_str(), _debug.length());
}

void _log::info(std::string _info) { 
   if (log_level < INFO) return ;
   char buf[128] = {0};
   sprintf(buf, "INFO : %s", _info.c_str());
   write(output_fd, _info.c_str(), _info.length());
}

void _log::_debug(const char *fmt, ...) {
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
         write(output_fd, buf, strlen(buf));
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
