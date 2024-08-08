#ifndef SERVER_H
#define SERVER_H

#include "http.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_HOST_LENGTH 256
#define MAX_ROUTES 100
#define MAX_MIDDLEWARES 10

typedef struct HTTPServer {
  int socket_fd;
  uint16_t port;
  char host[MAX_HOST_LENGTH];
  int server_fd;
  HTTPRoute routes[MAX_ROUTES];
  size_t route_count;
  HTTPMiddleware middlewares[MAX_MIDDLEWARES];
  size_t middleware_count;
} HTTPServer;

typedef struct {
  int client_fd;
  HTTPServer *server;
} ClientHandlerArgs;

void sigint_handler(int sig);
HTTPServer init_server(const char *host, uint16_t port);
int create_and_bind_socket(uint16_t port);
void run_server(HTTPServer *server);
void handle_client(int client_fd, HTTPServer *server);
void handle_request(HTTPServer *server, HTTPRequest *request,
                    HTTPResponse *response);

void send_http_response(int client_fd, HTTPResponse *response);

void process_middlewares(HTTPServer *server, HTTPRequest *request,
                         HTTPResponse *response);
void default_404(HTTPResponse *response);
void log_request(HTTPRequest *request);
void destroy_server(HTTPServer *server);

int add_route(HTTPServer *server, HTTPMethods method, const char *path,
              RouteHandler handler);
int add_middleware(HTTPServer *server, RouteHandler handler);
void *client_handler_thread(void *arg);
HTTPRoute *find_route(HTTPServer *server, HTTPMethods method, const char *path);

int create_thread_and_handle(int client_fd, HTTPServer *server);

#endif
