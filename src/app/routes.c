#include "routes.h"
#include "db.h"

sqlite3 *db = NULL;

void setup_routes(HTTPServer *server) {
  init_db();
  create_tables();

  add_route(server, GET, "/", home_handler);
  add_route(server, GET, "/about", about_handler);
  add_route(server, GET, "/blog", blog_handler);
  add_route(server, GET, "/projects", projects_handler);
  add_route(server, POST, "/submit", submit_handler);
  add_route(server, GET, "/static/image/alfie.jpeg", static_file_handler);

  get_all_blog_posts_and_set_routes(server);
}

void setup_middlewares(HTTPServer *server) { add_middleware(server, logger); }

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
    char *blog_list_html = generate_blog_list_html();

    char *placeholder = strstr(html_content, "{{ BLOG_POSTS }}");
    if (placeholder) {
      size_t prefix_len = placeholder - html_content;
      size_t suffix_len = strlen(placeholder + strlen("{{ BLOG_POSTS }}"));

      size_t new_len = prefix_len + strlen(blog_list_html) + suffix_len + 1;
      char *new_content = malloc(new_len);

      strncpy(new_content, html_content, prefix_len);
      strcpy(new_content + prefix_len, blog_list_html);
      strcpy(new_content + prefix_len + strlen(blog_list_html),
             placeholder + strlen("{{ BLOG_POSTS }}"));

      set_response_body(response, new_content, strlen(new_content));
      add_response_header(response, "Content-Type", "text/html");

      free(new_content);
      free(blog_list_html);
    } else {
      set_response_body(response, html_content, strlen(html_content));
      add_response_header(response, "Content-Type", "text/html");
    }

    free(html_content);
  } else {
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

  // Find the corresponding blog post
  BlogPost *found_post = NULL;
  for (int i = 0; i < post_count; i++) {
    if (strcmp(blog_posts[i].url_friendly_name, post_name) == 0) {
      found_post = &blog_posts[i];
      break;
    }
  }

  if (found_post == NULL) {
    const char *error_msg =
        "<html><body><h1>Error: Post not found</h1></body></html>";
    set_response_body(response, error_msg, strlen(error_msg));
    response->status_code = 404;
    strcpy(response->status_message, "Not Found");
    return;
  }

  // Construct the full path to the markdown file
  char file_path[512];
  snprintf(file_path, sizeof(file_path),
           "/home/alfie/.config/fifisv/hub/Home/StudyHall/Blog/%s",
           found_post->filename);

  // Read the markdown content
  char *md_content = read_file(file_path);
  if (md_content == NULL) {
    // Handle error - file not found
    const char *error_msg =
        "<html><body><h1>Error: Post content not found</h1></body></html>";
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

  char *template = read_file("templates/post.html");
  char *placeholder = strstr(template, "{{ BLOG_POST }}");
  if (placeholder) {
    size_t prefix_len = placeholder - template;
    size_t suffix_len = strlen(placeholder + strlen("{{ BLOG_POST }}"));

    size_t new_len = prefix_len + strlen(html_content) + suffix_len + 1;
    char *new_content = malloc(new_len);

    strncpy(new_content, template, prefix_len);
    strcpy(new_content + prefix_len, html_content);
    strcpy(new_content + prefix_len + strlen(html_content),
           placeholder + strlen("{{ BLOG_POST }}"));

    set_response_body(response, new_content, strlen(new_content));
    add_response_header(response, "Content-Type", "text/html");

    free(new_content);
  } else {
    const char *error_msg = "<html><body><h1>Error: Could not load page "
                            "template</h1></body></html>";
    set_response_body(response, error_msg, strlen(error_msg));
    response->status_code = 500;
    strcpy(response->status_message, "Internal Server Error");
  }

  free(html_content);
  free(template);
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
