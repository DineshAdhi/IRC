#include<stdlib.h>
#include<stdio.h> 
#include<string.h>
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<signal.h>

#include"../../include/commonutil.h"
#include"../../include/aes256.h"
#include"../../include/sha256.h"
#include "../../include/clientutil.h"
#include "../../include/log.h"

void handle_stdin_data()
{
      char *buffer = prompt("");

      if(strcmp("clear", buffer) == 0)
      {
            system("@cls||clear");
      }

      if(strcmp("ls", buffer) == 0)
      {
            system("ls");
      }

      if(strcmp("exit", buffer) == 0)
      {
            deregisterServer();
            exit(1);
      }

      if(strcmp("cmd", buffer) == 0)
      {
            system("python $PWD/otp.py");
      }
}

int initiateReconnect()
{
      log_info("[IRCCLIENT][SERVER CONNECTION LOST][RECONNECTING]");

      int itr = MAX_CONNECT_RETRY;;

      while(TRUE && itr >= 0)
      {
            int result = connect_to_server(RECONNECT);

            if(result == SUCCESS)
            {
                  log_info("[IRCSERVER CONNECTIONG RESTORED][RECONNECTING SUCCESS]");
                  return SUCCESS;
            }
            else
            {
                  printf("..");
                  fflush(stdout);
            }

            itr--;
            
            sleep(2);
      }

      return FAILURE;
}

void handle_io_client_handshake()
{
      switch(serverconn->stage)
      {
            case MESSAGE_TYPE__clienthello:
            {
                  IRCMessage *message = (IRCMessage *) calloc(1, sizeof(IRCMessage));
                  ircmessage__init(message);
                  message->key = (char *) createDFHKey(serverconn->randomkey);
                  serverconn->stage = MESSAGE_TYPE__serverhello;
                  wrapConnection(serverconn, message);

                  if(writeconnection(serverconn) == FAILURE)
                  {
                        deregisterServer();
                  }

                  break;
            }      

            case MESSAGE_TYPE__serverhello: 
            {
                  if(readconnection(serverconn, MESSAGE_TYPE__keyexchange) == SUCCESS)
                  {
                        serverconn->oppdfhkey = (uint8_t *) serverconn->payload->data->key;
                        serverconn->sharedkey = resolveDFHKey(serverconn->randomkey, serverconn->oppdfhkey);
                        log_debug("[CLIENT SHARED KEY]"); printKey(serverconn->sharedkey, KEYLENGTH);
                        serverconn->stage = MESSAGE_TYPE__keyexchange;
                        serverconn->writable = WRITABLE;
                  }
                  else 
                  {
                        log_error("[ERROR WHILE READING FROM CONNECTION]");
                        deregisterServer();
                  }

                  break;
            }

            case MESSAGE_TYPE__keyexchange:
            {
                  IRCMessage *msg = (IRCMessage *) calloc(1, sizeof(IRCMessage));
                  ircmessage__init(msg);
                  msg->key =(char *) serverconn->sharedkey;
                  
                  serverconn->secure = SECURE;
                  serverconn->aeswrapper = init_aes256_wrapper(serverconn->sharedkey);
                  serverconn->stage = MESSAGE_TYPE__handshakedone;
                  wrapConnection(serverconn, msg);

                  if(writeconnection(serverconn) == SUCCESS)
                  {
                        log_debug("[SHARED KEY EXCHANGE DONE]");
                  }
                  else 
                  {
                        log_error("[HANDSHAKE EXCEPTION][ERROR WHILE KEY EXCHANGE]");
                        deregisterServer();
                  }

                  break;
            }

            case MESSAGE_TYPE__handshakedone: 
            {
                  if(readconnection(serverconn, MESSAGE_TYPE__unknownstage) == SUCCESS)
                  {
                        serverconn->securekey = (uint8_t *) serverconn->payload->data->key;
                        serverconn->aeswrapper = init_aes256_wrapper(serverconn->securekey);
                        //serverconn->stage = MESSAGE_TYPE__unknownstage;
                        serverconn->handshakedone = HANDSHAKE_DONE;
                        log_debug("[RECEIVED MASTER SECRET]");
                        serverconn->writable = WRITABLE;
                        printKey(serverconn->securekey, KEYLENGTH);
                        printMessage("Connected to IRCServer [HANDSHAKE SUCCESFULLY ATTEMPTED]");
                  }
                  else 
                  {
                        log_error("[ERROR WHILE READING FROM CONNECTION]");
                        deregisterServer();
                  }

                  break;
            }

            default:
            {
                  log_error("[DEFAULT INVOKED - IRCSERVER DISCONNECTED DURING HANDSHAKE]");
                  deregisterServer();
                  break;
            }
      }
}

void signupUser()
{
      UserConfig *signupconfig = (UserConfig *) calloc(1, sizeof(UserConfig));
      user_config__init(signupconfig);
      signupconfig->id = prompt(">> UserID : ");
      signupconfig->password = sha256(prompt(">> Password : "));

      IRCMessage *msg = (IRCMessage *) calloc(1, sizeof(IRCMessage));
      ircmessage__init(msg);
      msg->userconfig = signupconfig;
      serverconn->stage = MESSAGE_TYPE__signup;
      wrapConnection(serverconn, msg);

      if(writeconnection(serverconn) == SUCCESS)
      {
            log_info("[SIGN UP][Userid - %s][Pass - %s]", signupconfig->id, signupconfig->password);
            printMessage("[User Id - %s][Sign up call sent]", signupconfig->id);  
      }
      else 
      {
            log_error("[ERROR WHILE SENDING MESSAGE FOR AUTHENTICATION]");
            deregisterServer();
      }
}

void getUserDetails()
{
      if(isAuthRequired == REQUIRED)
      {    
            char *ch = prompt(">> New User (Y/N) : ");

            if( !strcmp(ch, "YES") || !strcmp(ch, "yes") || !strcmp(ch, "Y") || !strcmp(ch, "y"))
            {
                        signupUser();
                        return;
            }

            userconfig = (UserConfig *) calloc(1, sizeof(UserConfig));
            user_config__init(userconfig);
            userconfig->id = prompt(">> UserID : ");
            userconfig->password = sha256(prompt(">> Password : "));
            saveConfigFile();
      }

      log_info("[Userid - %s][Pass - %s]", userconfig->id, userconfig->password);
      printMessage("You are now logged in as %s. Your config file can be found in config/config.irc", userconfig->id);

      IRCMessage *msg = (IRCMessage *) calloc(1, sizeof(IRCMessage));
      ircmessage__init(msg);
      msg->userconfig = userconfig;
      serverconn->stage = MESSAGE_TYPE__auth;
      wrapConnection(serverconn, msg);

      if(writeconnection(serverconn) == SUCCESS)
      {
            log_info("[SENT MESSAGE FOR AUTHENTICATION]");   
      }
      else 
      {
            log_error("[ERROR WHILE SENDING MESSAGE FOR AUTHENTICATION]");
            deregisterServer();
      }
}



void handle_io_client()
{
      if(serverconn->registered == NOT_REGISTERED)
      {
            log_info("[EXCEPTION WHILE MAKING CONNECTION][CONNECTION NOT REGISTERED");
            deregisterServer();
      }

      if(serverconn->handshakedone == HANDSHAKE_NOT_DONE)
      {
            handle_io_client_handshake();
            return;
      }

      if(serverconn->authdone == UNAUTHENTICATED)
      {
            switch(serverconn->stage)
            {
                  case MESSAGE_TYPE__handshakedone:
                  {
                        getUserDetails();
                        break;
                  }

                  case MESSAGE_TYPE__auth:
                  {
                        break;
                  }

                  case MESSAGE_TYPE__signup:
                  {
                        break;
                  }
                  
                  default: 
                        break;
            }
      }
}