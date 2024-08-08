#include "templar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *wrap_with_braces(const char *word) {
  size_t len = strlen(word);
  char *result = malloc(len + 5);

  if (result == NULL) {
    return NULL;
  }

  sprintf(result, "{{%s}}", word);
  return result;
}

char *str_replace(const char *source, const char *search, const char *replace) {
  const char *pos = source;
  int count = 0;

  // Count occurrences of search string
  while ((pos = strstr(pos, search)) != NULL) {
    count++;
    pos += strlen(search);
  }

  if (count == 0) {
    return strdup(source);
  }

  size_t result_size =
      strlen(source) + (strlen(replace) - strlen(search)) * count + 1;

  char *result = malloc(result_size);
  if (result == NULL) {
    return NULL;
  }

  char *current = result;
  while (*source) {
    if (strstr(source, search) == source) {
      strcpy(current, replace);
      current += strlen(replace);
      source += strlen(search);
    } else {
      *current++ = *source++;
    }
  }
  *current = '\0';

  return result;
}

int insert_text(const char *text, const char *templar_keywork, char *html) {
  char *search = wrap_with_braces(templar_keywork);

  free(search);
  return 0;
}
char *get_keywords();
