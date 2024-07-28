#ifndef HTTP_H

#define HTTP_H
#include <stddef.h>

#define MAX_HEADERS 20
#define MAX_HEADER_LENGTH 1024
#define MAX_BODY_SIZE 1048576 // 1MB max body size
#define MAX_HEADER_RESPONSE_LENGTH 256
//
typedef enum { GET, POST, PUT, DELETE, UNKNOWN } HTTPMethods;

typedef struct {
  char method[10];
  char path[1024];
  char version[10];
  char headers[MAX_HEADERS][MAX_HEADER_LENGTH];
  HTTPMethods http_method;
  int header_count;
} HTTPRequest;

typedef struct {
  int status_code;
  char status_message[32];
  char headers[MAX_HEADERS][MAX_HEADER_RESPONSE_LENGTH];
  int header_count;
  char *body;
  size_t body_length;
} HTTPResponse;

typedef void (*RouteHandler)(HTTPRequest *request, HTTPResponse *response);

// Define the HTTPRoute struct
typedef struct {
  HTTPMethods method;
  char path[1024];
  RouteHandler handler;
} HTTPRoute;

// REQUEST DEFS
void parse_request_line(char *line, HTTPRequest *request);
void parse_header(char *line, HTTPRequest *request);
HTTPRequest *parse_http_request(int client_fd);
HTTPMethods parse_method_from_string(const char *method);
void destroy_http_request(HTTPRequest *request);

// RESPONSE DEFS
// Function to initialize an HTTPResponse
HTTPResponse *create_http_response();
// Function to add a header to the response
void add_response_header(HTTPResponse *response, const char *name,
                         const char *value);
// Function to set the response body
void set_response_body(HTTPResponse *response, const char *body, size_t length);
// Function to free the HTTPResponse
void destroy_http_response(HTTPResponse *response);
#endif // !HTTP_H
