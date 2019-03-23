#include <sqlite3.h> 
#include "commonutil.h"
#define AUTHDB_PATH "db/auth.db"
#define SIGNUP_INSERT "INSERT INTO USERS(ID,HASH) VALUES('%s','%s')"
#define AUTH_SELECT "SELECT * FROM USERS WHERE ID='%s' AND HASH='%s'"

sqlite3 *db;

void initialize_db();
void close_db();
void db_signupUser(Connection *c, char *id, char *hash);
void db_loginUser(Connection *c, char *id, char *hash);