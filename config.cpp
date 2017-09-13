#include "config.h"
#include "log.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
//#include <glib.h>

Config::Config(const char *config_file) {
   return ;
   /*
   int fd = open(config_file, O_CREAT | O_RDWR | O_APPEND, S_IRUSR|S_IWUSR|S_IXUSR);
   if (fd == -1) {
      LogUtil::debug("file : %s open error", config_file);
      return ;
   }
   GKeyFile *keyfile = g_key_file_new();
   GError *err = NULL;
   g_key_file_load_from_file(keyfile, (gchar *)(config_file), G_KEY_FILE_NONE, &err);
   char *port = g_key_file_get_string(keyfile, "stage", "port", &err);
   listen_port = atoi(port);
   */
   /*
   int fd = open(config_file, O_CREAT | O_RDWR | O_APPEND, S_IRUSR|S_IWUSR|S_IXUSR);
   if (fd == -1) {
      LogUtil::debug("config : [Config], file %s open fail. %s", config_file, strerror(errno));
      return ;
   }
   */

   FILE *fp = fopen(config_file, "r");
   if (!fp) {
      LogUtil::debug("config : [Config], file %s open fail. %s", config_file, strerror(errno));
      exit(-1);
   } 
   char buf[256];

   vector<stage_config > config;
   string res, des;
   while (true) {
      memset(buf, 0, 256);
      if (!fgets(buf, 256, fp)) {
         LogUtil::debug("config : [config], end of file or error");
         break;
      }
      if (buf[0] == 0x00 || buf[0] == '#') {
         // This is the empty line, we just continue.
         // '#' This is the comment of the line, just continue.
         continue;
      }
      char *p = strchr(buf, '=');
      *p = 0x00;
      if (memcmp(buf, "resource", 8) == 0) { 
         res = p + 1;
      } else if (memcmp(buf, "destination", strlen("destination")) == 0) {
         des = p + 1;
      } else {
         *p = '=';
         LogUtil::debug("config : [config] %s error formate", buf);
         exit(-1);
      }
      if (res.length() && des.length()) { 
         stage_config sc(res, des);
         config.push_back(sc);
         res.clear();
         des.clear();
      }
   }
   if (res.length() || des.length()) { 
      LogUtil::debug("config : [config] file %s formate error.", config_file);
      exit(-1);
   }
}
