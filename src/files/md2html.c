#include "md2html.h"
#include "string.h"
#include <md4c-html.h>
#include <md4c.h>
#include <stdlib.h>

// This function will be called by md_html to process output
static void process_output(const MD_CHAR *text, MD_SIZE size, void *userdata) {
  char **html = (char **)userdata;
  size_t current_len = *html ? strlen(*html) : 0;
  *html = realloc(*html, current_len + size + 1);
  if (*html) {
    memcpy(*html + current_len, text, size);
    (*html)[current_len + size] = '\0';
  }
}

char *markdown_to_html(const char *markdown) {
  char *html = NULL;

  // Call md_html to parse markdown and generate HTML
  int result = md_html(markdown, strlen(markdown), process_output, &html,
                       0, // parser_flags (0 for default options)
                       0  // renderer_flags (0 for default options)
  );

  if (result != 0) {
    free(html);
    return NULL;
  }

  return html;
}
