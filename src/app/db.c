#include "db.h"
#include <stdio.h>

int init_db() {
  FILE *fp = fopen(DB_NAME, "r");
  if (!fp) {
    // File does not exist, create it
    fp = fopen(DB_NAME, "w");
    if (!fp) {
      fprintf(stderr, "Error creating database file\n");
      return 1;
    }
    fclose(fp);
  }

  int rc = sqlite3_open(DB_NAME, &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  printf("Database opened successfully\n");
  return 0;
}

int create_tables() {
  char *sql = "CREATE TABLE IF NOT EXISTS page_activity ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "page TEXT NOT NULL, "
              "ip TEXT NOT NULL "
              ")";

  int rc = sqlite3_exec(db, sql, NULL, NULL, NULL);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Error creating table: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  printf("Tables created successfully\n");
  return 0;
}

void close_db() { sqlite3_close(db); }