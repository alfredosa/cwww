#include "read.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_html_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Error opening file: %s\n", filename);
    return NULL;
  }

  // Determine file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  // Allocate memory for file content
  char *content = malloc(file_size + 1);
  if (content == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    fclose(file);
    return NULL;
  }

  // Read file content
  size_t bytes_read = fread(content, 1, file_size, file);
  if (bytes_read < file_size) {
    fprintf(stderr, "Error reading file: %s\n", filename);
    free(content);
    fclose(file);
    return NULL;
  }

  // Null-terminate the string
  content[file_size] = '\0';

  fclose(file);
  return content;
}
