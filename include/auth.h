#include <sqlite3.h> 
#define AUTHDB_PATH "db/auth.db"
#define AUTH_INSERT "INSERT INTO USERS(ID,HASH) VALUES('%s','%s')"

sqlite3 *db;

void initialize_db();
void close_db();
void db_signupUser(char *id, char *hash);