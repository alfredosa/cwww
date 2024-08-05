#ifndef FILES_H
#define FILES_H

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_FILENAME 256
#define INITIAL_CAPACITY 10

char *read_file(const char *filename);

/* Function returns all files inside a directory (names only)
 * REQUIRED FREE(whatever this returns)*/

char **read_all_files(const char *path, int *file_count);
char* read_image(const char* file_path, size_t* size);

#endif // !FILES_H
