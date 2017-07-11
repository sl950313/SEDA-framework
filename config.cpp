#include "config.h"
#include "log.h"
#include <unistd.h>
#include <fcntl.h>
#include <glib.h>

Config::Config(const char *config_file) {
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
}
