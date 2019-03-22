#include<stdio.h>
#include<stdlib.h>
#include<sqlite3.h>

#include "../../include/auth.h"
#include "../../include/log.h"

void initialize_db()
{
      int res = sqlite3_open(AUTHDB_PATH, &db);

      if(res)
      {
            log_error("[DB INITIALIZATION FAILED]");
      }
      else 
      {
            log_info("[DB INITIALIZATION SUCCESSFUL]");
      }
}

void close_db()
{
      sqlite3_close(db);
      log_info("[DB CLOSED]");
}

static int _db_signup_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
      return 1;
}

void db_signupUser(char *id, char *hash)
{
      char *cmd = (char *) calloc(100, sizeof(char));
      char *err = (char *) calloc(100, sizeof(char));
      
      sprintf(cmd, "INSERT INTO USERS(ID,HASH) VALUES('%s','%s')", id, hash);

      log_info("[SIGNING UP]");

      sqlite3_exec(db, cmd, _db_signup_callback, 0, &err);
}



