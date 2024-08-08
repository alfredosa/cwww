#ifndef ROUTES_H
#define ROUTES_H

#include "../server/http.h"
#include "../server/server.h"

#include "../files/md2html.h"
#include "../files/read.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"

extern char **blog_posts;
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
void cleanup_blog_posts();

// MIDDLEWARES
void setup_middlewares(HTTPServer *server);

void logger(HTTPRequest *request, HTTPResponse *response);

void setup_routes(HTTPServer *server);
#endif