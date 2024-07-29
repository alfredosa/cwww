#include "read.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

char *read_file(const char *filename) {
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

char **read_all_files(const char *path, int *file_count) {
  DIR *dir;
  struct dirent *entry;
  struct stat file_stat;
  char full_path[1024];
  char **file_list = NULL;
  int capacity = INITIAL_CAPACITY;
  *file_count = 0;

  // Allocate initial memory for file_list
  file_list = malloc(capacity * sizeof(char *));
  if (file_list == NULL) {
    perror("Memory allocation failed");
    return NULL;
  }

  // Open the directory
  dir = opendir(path);
  if (dir == NULL) {
    perror("Error opening directory");
    free(file_list);
    return NULL;
  }

  // Read directory entries
  while ((entry = readdir(dir)) != NULL) {
    // Construct the full path
    snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

    // Get file status
    if (stat(full_path, &file_stat) < 0) {
      perror("Error getting file status");
      continue;
    }

    // Check if it's a regular file (not a directory)
    if (S_ISREG(file_stat.st_mode)) {
      // Reallocate memory if needed
      if (*file_count >= capacity) {
        capacity *= 2;
        char **temp = realloc(file_list, capacity * sizeof(char *));
        if (temp == NULL) {
          perror("Memory reallocation failed");
          break;
        }
        file_list = temp;
      }

      // Allocate memory for the filename and copy it
      file_list[*file_count] = malloc(MAX_FILENAME * sizeof(char));
      if (file_list[*file_count] == NULL) {
        perror("Memory allocation failed");
        break;
      }
      strncpy(file_list[*file_count], entry->d_name, MAX_FILENAME - 1);
      file_list[*file_count][MAX_FILENAME - 1] =
          '\0'; // Ensure null-termination
      (*file_count)++;
    }
  }

  closedir(dir);

  return file_list;
}
