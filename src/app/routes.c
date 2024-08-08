#include "routes.h"
#include "db.h"

sqlite3 *db = NULL;
char **blog_posts = NULL;
int post_count = 0;

void setup_routes(HTTPServer *server) {
  // TODO: we need middlewares :D

  init_db();
  create_tables();

  // Add routes
  add_route(server, GET, "/", home_handler);
  add_route(server, GET, "/about", about_handler);
  add_route(server, GET, "/blog", blog_handler);
  add_route(server, GET, "/projects", projects_handler);
  add_route(server, POST, "/submit", submit_handler);

  add_route(server, GET, "/static/image/alfie.jpeg", static_file_handler);
  get_all_blog_posts_and_set_routes(server);
}

void setup_middlewares(HTTPServer *server) { add_middleware(server, logger); }

void get_all_blog_posts_and_set_routes(HTTPServer *server) {
  const char *path = "static/posts";
  blog_posts = read_all_files(path, &post_count);
  if (blog_posts != NULL) {
    for (int i = 0; i < post_count; i++) {
      printf("%s\n", blog_posts[i]);
      char route[256];
      snprintf(route, sizeof(route), "/blog/%s", blog_posts[i]);
      // Remove the .md extension if present
      char *dot = strrchr(route, '.');
      if (dot && strcmp(dot, ".md") == 0) {
        *dot = '\0';
      }
      add_route(server, GET, route, blog_post);
    }
  }
}

void projects_handler(HTTPRequest *request, HTTPResponse *response) {
  char *html_content = read_file("templates/projects.html");
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

void static_file_handler(HTTPRequest *request, HTTPResponse *response) {
  // Extract the file path from the request
  const char *file_path = request->path + 1; // Skip the leading '/'

  // Read the file content
  size_t file_size;
  char *file_content = read_image(file_path, &file_size);
  if (file_content) {
    // Set the response body
    set_response_body(response, file_content, file_size);

    // Set the Content-Type header based on the file extension
    const char *extension = strrchr(file_path, '.');
    if (extension) {
      if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) {
        add_response_header(response, "Content-Type", "image/jpeg");
      } else if (strcmp(extension, ".png") == 0) {
        add_response_header(response, "Content-Type", "image/png");
      } else if (strcmp(extension, ".css") == 0) {
        add_response_header(response, "Content-Type", "text/css");
      } else if (strcmp(extension, ".js") == 0) {
        add_response_header(response, "Content-Type", "application/javascript");
      } else {
        add_response_header(response, "Content-Type",
                            "application/octet-stream");
      }
    }

    free(file_content);
  } else {
    // File not found
    const char *error_msg =
        "<html><body><h1>Error: File not found</h1></body></html>";
    set_response_body(response, error_msg, strlen(error_msg));
    response->status_code = 404;
    strcpy(response->status_message, "Not Found");
  }
}

void cleanup_blog_posts() {
  if (blog_posts != NULL) {
    for (int i = 0; i < post_count; i++) {
      free(blog_posts[i]);
    }
    free(blog_posts);
    blog_posts = NULL;
  }
}
