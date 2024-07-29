

#include "files/md2html.h"
#include "files/read.h"
#include "server/http.h"
#include "server/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void home_handler(HTTPRequest *request, HTTPResponse *response);
void blog_handler(HTTPRequest *request, HTTPResponse *response);
void about_handler(HTTPRequest *request, HTTPResponse *response);
void submit_handler(HTTPRequest *request, HTTPResponse *response);
void blog_post(HTTPRequest *request, HTTPResponse *response);

int main() {
  printf("INFO: life of my blog is live\n");

  HTTPServer server = init_server("localhost", 8080);

  // Add routes
  add_route(&server, GET, "/", home_handler);
  add_route(&server, GET, "/about", about_handler);
  add_route(&server, GET, "/blog", blog_handler);
  add_route(&server, POST, "/submit", submit_handler);

  const char *path = "static/posts";
  int file_count;
  char **files = read_all_files(path, &file_count);
  if (files != NULL) {
    for (int i = 0; i < file_count; i++) {
      printf("%s\n", files[i]);
      char route[256];
      snprintf(route, sizeof(route), "/blog/%s", files[i]);
      // Remove the .md extension if present
      char *dot = strrchr(route, '.');
      if (dot && strcmp(dot, ".md") == 0) {
        *dot = '\0';
      }
      add_route(&server, GET, route, blog_post);
      printf("added post route %s\n", files[i]);
      free(files[i]);
    }
    free(files);
  }

  run_server(&server);

  destroy_server(&server);
  printf("See You Cowboy....\n");
  return 0;
}

void blog_handler(HTTPRequest *request, HTTPResponse *response) {
  char *html_content = read_file("templates/index.html");
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

void blog_post(HTTPRequest *request, HTTPResponse *response) {
  // Extract the post name from the path
  const char *post_name = strrchr(request->path, '/');
  if (post_name == NULL) {
    // Handle error - invalid path
    const char *error_msg =
        "<html><body><h1>Error: Invalid post path</h1></body></html>";
    set_response_body(response, error_msg, strlen(error_msg));
    response->status_code = 400;
    strcpy(response->status_message, "Bad Request");
    return;
  }
  post_name++; // Skip the '/'

  // Construct the full path to the markdown file
  char file_path[512];
  snprintf(file_path, sizeof(file_path), "static/posts/%s.md", post_name);

  // Read the markdown content
  char *md_content = read_file(file_path);
  if (md_content == NULL) {
    // Handle error - file not found
    const char *error_msg =
        "<html><body><h1>Error: Post not found</h1></body></html>";
    set_response_body(response, error_msg, strlen(error_msg));
    response->status_code = 404;
    strcpy(response->status_message, "Not Found");
    return;
  }

  // Convert markdown to HTML (you'll need to implement this function)
  char *html_content = markdown_to_html(md_content);
  free(md_content);

  if (html_content == NULL) {
    // Handle error - conversion failed
    const char *error_msg =
        "<html><body><h1>Error: Could not process post</h1></body></html>";
    set_response_body(response, error_msg, strlen(error_msg));
    response->status_code = 500;
    strcpy(response->status_message, "Internal Server Error");
    return;
  }

  // Set the response
  set_response_body(response, html_content, strlen(html_content));
  add_response_header(response, "Content-Type", "text/html");
  free(html_content);
}

void home_handler(HTTPRequest *request, HTTPResponse *response) {
  char *html_content = read_file("templates/home.html");
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
