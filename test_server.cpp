#include "tcp_server.h"

int main() {
   tcp_server *ser = new tcp_server("sl_server");
   ser->loop();
   ser->log->debug("End\n");
}

