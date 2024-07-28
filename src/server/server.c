#include "server.h"
#include "http.h"
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

HTTPServer init_server(const char *host, uint16_t port) {
  HTTPServer server;
  server.port = port;
  strncpy(server.host, host, sizeof(server.host) - 1);
  server.host[sizeof(server.host) - 1] = '\0';

  server.socket_fd = create_and_bind_socket(port);
  if (server.socket_fd < 0) {
    fprintf(stderr, "Failed to create and bind socket\n");
    exit(EXIT_FAILURE);
  }

  if (listen(server.socket_fd, SOMAXCONN) < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  server.route_count = 0;
  return server;
}

int create_and_bind_socket(uint16_t port) {
  int server_fd;
  struct sockaddr_in address;
  int opt = 1;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failed");
    return -1;
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt SO_REUSEADDR");
    close(server_fd);
    return -1;
  }

#ifdef SO_REUSEPORT
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    // This is non-critical, so just print a warning
    // Seems like REUSEPORT is not available in amakell architectures.
    perror("Warning: setsockopt SO_REUSEPORT");
  }
#endif

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    return -1;
  }

  return server_fd;
}

void run_server(HTTPServer *server) {
  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd =
        accept(server->socket_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
      perror("accept failed");
      continue;
    }

    handle_client(client_fd, server);
  }
}

void handle_client(int client_fd, HTTPServer *server) {

  HTTPRequest *request = parse_http_request(client_fd);
  HTTPResponse *response = create_http_response();

  // TODO: Parse request, generate response
  // TODO: Missing concurrency. Spawn threads, destroy them. Be happy, own
  // nothing kinda meme.

  log_request(request);
  handle_request(server, request, response);

  send_http_response(client_fd, response);

  destroy_http_response(response);
  destroy_http_request(request);
  close(client_fd);
}

void destroy_server(HTTPServer *server) {
  if (server) {
    if (server->socket_fd != -1) {
      close(server->socket_fd);
      server->socket_fd = -1;
    }
    // TODO: ALFIE: If you've dynamically allocated any memory, free it here
  }
}

void handle_request(HTTPServer *server, HTTPRequest *request,
                    HTTPResponse *response) {
  if (!request) {
    default_404(response);
    return;
  }

  HTTPRoute *route = find_route(server, request->http_method, request->path);
  if (route) {
    route->handler(request, response);
  } else {
    default_404(response);
    return;
  }
}

void default_404(HTTPResponse *response) {
  response->status_code = 404;
  strcpy(response->status_message, "Not Found");
  add_response_header(response, "Content-Type", "text/html");
  const char *body =
      "<html><body><h1>OOOPPS, looks like you wandered to a strange "
      "place</h1>\n<p>404 Unrecognized Request</p></body></html>";
  set_response_body(response, body, strlen(body));
}

void log_request(HTTPRequest *request) {
  if (request)
    printf("INFO: %s, %s, headers #: %i\n", request->method, request->path,
           request->header_count);
}

// Function to add a route to the server
int add_route(HTTPServer *server, HTTPMethods method, const char *path,
              RouteHandler handler) {
  if (server->route_count >= MAX_ROUTES) {
    return -1;
  }

  HTTPRoute *route = &server->routes[server->route_count];
  route->method = method;
  strncpy(route->path, path, sizeof(route->path) - 1);
  route->path[sizeof(route->path) - 1] = '\0'; // Ensure null-termination
  route->handler = handler;

  server->route_count++;
  return 0;
}

// Function to find a matching route
HTTPRoute *find_route(HTTPServer *server, HTTPMethods method,
                      const char *path) {
  for (size_t i = 0; i < server->route_count; i++) {
    if (server->routes[i].method == method &&
        strcmp(server->routes[i].path, path) == 0) {
      return &server->routes[i];
    }
  }
  return NULL; // No matching route found
}

void send_http_response(int client_fd, HTTPResponse *response) {
  char buffer[1024];
  int len = snprintf(buffer, sizeof(buffer), "HTTP/1.1 %d %s\r\n",
                     response->status_code, response->status_message);
  write(client_fd, buffer, len);

  for (int i = 0; i < response->header_count; i++) {
    write(client_fd, response->headers[i], strlen(response->headers[i]));
    write(client_fd, "\r\n", 2);
  }

  len = snprintf(buffer, sizeof(buffer), "Content-Length: %zu\r\n\r\n",
                 response->body_length);
  write(client_fd, buffer, len);

  if (response->body && response->body_length > 0) {
    write(client_fd, response->body, response->body_length);
  }
}
