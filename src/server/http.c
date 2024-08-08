#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void parse_request_line(char *line, HTTPRequest *request) {
  sscanf(line, "%9s %1023s %9s", request->method, request->path,
         request->version);
}

void parse_header(char *line, HTTPRequest *request) {
  if (request->header_count < MAX_HEADERS) {
    strncpy(request->headers[request->header_count], line,
            MAX_HEADER_LENGTH - 1);
    request->headers[request->header_count][MAX_HEADER_LENGTH - 1] = '\0';
    request->header_count++;
  }
}

void initialize_http_request(HTTPRequest *request) {
  if (request) {
    memset(request, 0, sizeof(HTTPRequest));
    request->method[0] = '\0';
    request->path[0] = '\0';
    request->version[0] = '\0';
    request->http_method = UNKNOWN;
    request->header_count = 0;
    request->client_ip[0] = '\0';
  }
}

HTTPRequest *parse_http_request(int client_fd) {
  char buffer[1024] = {0};
  ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
  HTTPRequest *request = malloc(sizeof(HTTPRequest));
  if (request == NULL) {
    return NULL; // Handle malloc failure
  }

  initialize_http_request(request);

  // Get client IP address
  struct sockaddr_in addr;
  socklen_t addr_size = sizeof(struct sockaddr_in);
  getpeername(client_fd, (struct sockaddr *)&addr, &addr_size);
  inet_ntop(AF_INET, &(addr.sin_addr), request->client_ip, INET6_ADDRSTRLEN);

  if (bytes_read > 0) {
    char *line = strtok(buffer, "\r\n");
    if (line) {
      parse_request_line(line, request);
      line = strtok(NULL, "\r\n");
    }
    while (line) {
      parse_header(line, request);
      line = strtok(NULL, "\r\n");
    }
    request->http_method = parse_method_from_string(request->method);
    return request;
  }

  // Handle invalid response
  free(request);
  return NULL;
}

void destroy_http_request(HTTPRequest *request) {
  if (request) {
    free(request);
  }
}

HTTPMethods parse_method_from_string(const char *method) {
  if (strcmp(method, "GET") == 0) {
    return GET;
  } else if (strcmp(method, "POST") == 0) {
    return POST;
  } else if (strcmp(method, "PUT") == 0) {
    return PUT;
  } else if (strcmp(method, "DELETE") == 0) {
    return DELETE;
  } else {
    return UNKNOWN;
  }
}

HTTPResponse *create_http_response() {
  HTTPResponse *response = malloc(sizeof(HTTPResponse));
  if (response) {
    response->status_code = 200;
    strcpy(response->status_message, "OK");
    response->header_count = 0;
    response->body = NULL;
    response->body_length = 0;
  }
  return response;
}

// Function to add a header to the response
void add_response_header(HTTPResponse *response, const char *name,
                         const char *value) {
  if (response->header_count < MAX_HEADERS) {
    snprintf(response->headers[response->header_count], MAX_HEADER_LENGTH,
             "%s: %s", name, value);
    response->header_count++;
  }
}

// Function to set the response body
void set_response_body(HTTPResponse *response, const char *body,
                       size_t length) {
  if (length > MAX_BODY_SIZE) {
    length = MAX_BODY_SIZE;
  }
  response->body = malloc(length);
  if (response->body) {
    memcpy(response->body, body, length);
    response->body_length = length;
  }
}

// Function to free the HTTPResponse
void destroy_http_response(HTTPResponse *response) {
  if (response) {
    free(response->body);
    free(response);
  }
}
