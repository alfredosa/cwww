#include "md2html.h"
#include "string.h"
#include <stdlib.h>

char *markdown_to_html(const char *markdown) {
  // This is a very simplistic conversion and doesn't handle most Markdown
  // features
  char *html = malloc(strlen(markdown) * 2); // Allocate more than needed
  if (html == NULL)
    return NULL;

  char *write_ptr = html;
  const char *read_ptr = markdown;

  *write_ptr++ = '<';
  *write_ptr++ = 'p';
  *write_ptr++ = '>';

  while (*read_ptr) {
    if (*read_ptr == '\n' && *(read_ptr + 1) == '\n') {
      strcpy(write_ptr, "</p><p>");
      write_ptr += 7;
      read_ptr += 2;
    } else {
      *write_ptr++ = *read_ptr++;
    }
  }

  strcpy(write_ptr, "</p>");
  write_ptr += 4;
  *write_ptr = '\0';

  return html;
}
