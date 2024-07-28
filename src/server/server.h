#ifndef SERVER_H
#define SERVER_H

#include "http.h"
#include <stdint.h>

#define MAX_HOST_LENGTH 256
#define MAX_ROUTES 100

typedef struct {
  int socket_fd;
  uint16_t port;
  char host[MAX_HOST_LENGTH];
  int server_fd;
  HTTPRoute routes[MAX_ROUTES];
  size_t route_count;
} HTTPServer;

HTTPServer init_server(const char *host, uint16_t port);
int create_and_bind_socket(uint16_t port);
void run_server(HTTPServer *server);
void handle_client(int client_fd, HTTPServer *server);
void handle_request(HTTPServer *server, HTTPRequest *request,
                    HTTPResponse *response);

void send_http_response(int client_fd, HTTPResponse *response);

void default_404(HTTPResponse *response);
void log_request(HTTPRequest *request);
void destroy_server(HTTPServer *server);

int add_route(HTTPServer *server, HTTPMethods method, const char *path,
              RouteHandler handler);
HTTPRoute *find_route(HTTPServer *server, HTTPMethods method, const char *path);
#endif
