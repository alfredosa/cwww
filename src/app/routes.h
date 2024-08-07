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

void home_handler(HTTPRequest *request, HTTPResponse *response);
void blog_handler(HTTPRequest *request, HTTPResponse *response);
void about_handler(HTTPRequest *request, HTTPResponse *response);
void submit_handler(HTTPRequest *request, HTTPResponse *response);
void blog_post(HTTPRequest *request, HTTPResponse *response);
void projects_handler(HTTPRequest *request, HTTPResponse *response);
void static_file_handler(HTTPRequest *request, HTTPResponse *response);

void setup_routes(HTTPServer *server);
#endif