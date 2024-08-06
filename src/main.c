#include "app/routes.h"

int main() {
  printf("INFO: life of my blog is live\n");

  HTTPServer server = init_server("localhost", 8080);
  setup_routes(&server);

  run_server(&server);

  destroy_server(&server);
  printf("See You Cowboy....\n");
  return 0;
}
