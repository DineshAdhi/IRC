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

int _db_auth_callback(void* m, int argc, char** argv, char** s)
{
      int i;
      IRCMessage *msg = (IRCMessage *) m;
      char *id = msg->userconfig->id;
      char *hash = msg->userconfig->password;
      
      if(argc == 2 && strcmp(id, argv[0]) == 0 && strcmp(hash, argv[1]) == 0)
      {
            msg->authstatus = DB_AUTH_SUCCESS;
      }
      else 
      {
            msg->authstatus = DB_AUTH_FAILED;
      }

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

      UserConfig* userconf = (UserConfig *) calloc(1, sizeof(UserConfig));
      user_config__init(userconf);
      userconf->id = id;
      userconf->password = hash;

      IRCMessage *msg = (IRCMessage *) calloc(1, sizeof(IRCMessage));
      ircmessage__init(msg);
      msg->authstatus = DB_AUTH_FAILED;
      msg->userconfig = userconf;

      int res = sqlite3_exec(db, cmd, _db_auth_callback, msg , &err);

      if(msg->authstatus == DB_AUTH_SUCCESS)
      {
            log_debug("[LOGIN AUTH SUCCESS][%s]", id);
      }
      else 
      {
            log_debug("[LOGIN AUTH FAILED][%s]", id);
      }

      c->stage = MESSAGE_TYPE__auth;
      wrapConnection(c, msg);
      c->writable = WRITABLE;
}



