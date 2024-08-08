#include "db.h"
#include "routes.h"

void logger(HTTPRequest *request, HTTPResponse *response) {
  log_request(request);
}

void analytics(HTTPRequest *request, HTTPResponse *response) {}