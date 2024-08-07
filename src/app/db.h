#ifndef DB_H
#define DB_H

#include <sqlite3.h>
#define DB_NAME "alfie.db"

extern sqlite3 *db;

int init_db();
int create_tables();
void close_db();

#endif // !DB_H