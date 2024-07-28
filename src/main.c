

#include "files/read.h"
#include "server/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void home_handler(HTTPRequest *request, HTTPResponse *response);
void about_handler(HTTPRequest *request, HTTPResponse *response);
void submit_handler(HTTPRequest *request, HTTPResponse *response);

int main() {
  printf("INFO: life of my blog is live\n");

  HTTPServer server = init_server("localhost", 8080);

  // Add routes
  add_route(&server, GET, "/", home_handler);
  add_route(&server, GET, "/about", about_handler);
  add_route(&server, POST, "/submit", submit_handler);

  run_server(&server);

  destroy_server(&server);
  printf("See You Cowboy....\n");
  return 0;
}

void home_handler(HTTPRequest *request, HTTPResponse *response) {
  char *html_content = read_html_file("templates/index.html");
  if (html_content) {
    set_response_body(response, html_content, strlen(html_content));
    add_response_header(response, "Content-Type", "text/html");
    free(html_content);
  } else {
    // Handle error - maybe set a default response or a 500 Internal Server
    // Error
    const char *error_msg =
        "<html><body><h1>Error: Could not load page</h1></body></html>";
    set_response_body(response, error_msg, strlen(error_msg));
    response->status_code = 500;
    strcpy(response->status_message, "Internal Server Error");
  }
}

void about_handler(HTTPRequest *request, HTTPResponse *response) {
  const char *body = "<html><body><h1>About Us</h1><p>This is a simple C web "
                     "server.</p></body></html>";
  set_response_body(response, body, strlen(body));
  add_response_header(response, "Content-Type", "text/html");
}

void submit_handler(HTTPRequest *request, HTTPResponse *response) {
  const char *body = "<html><body><h1>Submission Received</h1><p>Thank you for "
                     "your submission.</p></body></html>";
  set_response_body(response, body, strlen(body));
  add_response_header(response, "Content-Type", "text/html");
  response->status_code = 201; // Created
  strcpy(response->status_message, "Created");
}
