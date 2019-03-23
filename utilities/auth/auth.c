#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sqlite3.h>

#include "../../include/auth.h"
#include "../../include/log.h"
#include "../../include/commonutil.h"

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

int _db_auth_callback(void* c, int argc, char** argv, char** s)
{
      int i;
      Connection *conn = (Connection *) c;
      char *id = conn->payload->data->userconfig->id;
      char *hash = conn->payload->data->userconfig->password;

      IRCMessage *msg = (IRCMessage *) calloc(1, sizeof(IRCMessage));
      ircmessage__init(msg);
      msg->authstatus = DB_AUTH_FAILED;

      for(i=0; i<argc; i++)
      {
            log_debug("%s", argv[i]);
      }
      

      if(argc == 2 && strcmp(id, argv[0]) == 0 && strcmp(hash, argv[1]) == 0)
      {
            msg->authstatus = DB_AUTH_SUCCESS;
            log_debug("[DB LOGIN AUTH SUCCESS]");
      }
      else 
      {
            msg->authstatus = DB_AUTH_FAILED;
            log_debug("[DB LOGIN AUTH FAILED]");
      }

      conn->stage = MESSAGE_TYPE__auth;
      wrapConnection(c, msg);
      conn->writable = WRITABLE;

      return 1;
}

void db_signupUser(Connection *conn, char *id, char *hash)
{
      char *cmd = (char *) calloc(100, sizeof(char));
      char *err = (char *) calloc(100, sizeof(char));

      sprintf(cmd, SIGNUP_INSERT, id, hash);

      int res = sqlite3_exec(db, cmd, 0, 0, &err);

      IRCMessage *msg = (IRCMessage *)calloc(1, sizeof(IRCMessage));
      ircmessage__init(msg);
      msg->authstatus = (res == SQLITE_OK) ? DB_SIGNUP_SUCCESS : DB_SIGNUP_FAILED;
      
      if(msg->authstatus == DB_SIGNUP_FAILED)
      {
            log_info("[SIGNING UP FAILED][%s][%s]", err, id);
      }
      else
      {
            log_info("[SIGNING UP SUCCESS][%s][%s]", id, hash);
      }

      conn->stage = MESSAGE_TYPE__signup;
      wrapConnection(conn, msg);
      conn->writable = WRITABLE;
}

void db_loginUser(Connection *c, char *id, char *hash)
{
      char *cmd = (char *) calloc(100, sizeof(char));
      char *err = (char *) calloc(100, sizeof(char));

      sprintf(cmd, AUTH_SELECT, id, hash);

      int res = sqlite3_exec(db, cmd, _db_auth_callback, c, &err);

      log_debug("[RES][%d][%s]", res == SQLITE_OK, err);
}



