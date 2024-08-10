#ifndef ROUTES_H
#define ROUTES_H

#include "../server/http.h"
#include "../server/server.h"

#include "../files/md2html.h"
#include "../files/read.h"

#include "../templar/templar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"

typedef struct {
  char *filename;
  char *url_friendly_name;
  char *created_date;
  char *published_date;
  char *nice_date;
  char **tags;
  int tag_count;
} BlogPost;

extern BlogPost *blog_posts;
extern int post_count;

void home_handler(HTTPRequest *request, HTTPResponse *response);
void blog_handler(HTTPRequest *request, HTTPResponse *response);
void about_handler(HTTPRequest *request, HTTPResponse *response);
void submit_handler(HTTPRequest *request, HTTPResponse *response);
void blog_post(HTTPRequest *request, HTTPResponse *response);
void projects_handler(HTTPRequest *request, HTTPResponse *response);
void static_file_handler(HTTPRequest *request, HTTPResponse *response);

// Blog Posts Related
void get_all_blog_posts_and_set_routes(HTTPServer *server);
char *prep_blog_posts(char *keyword);
void free_blog_post(BlogPost *post);
char *format_nice_date(const char *iso_date);
char *extract_metadata(const char *content, const char *key);
char *generate_blog_list_html();
char **extract_tags(const char *content, int *tag_count);

// MIDDLEWARES
void setup_middlewares(HTTPServer *server);

void logger(HTTPRequest *request, HTTPResponse *response);

void setup_routes(HTTPServer *server);
#endif