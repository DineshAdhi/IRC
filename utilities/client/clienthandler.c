#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<signal.h>

#include"../common/commonutil.h"
#include"../crypto/aes256.h"
#include "clientutil.h"
#include "../logger/log.h"

void handle_stdin_data()
{
      char *stdinbuffer = (char *) calloc(MAX_STDIN_INPUT, sizeof(char));
      int bytes = read(STDIN_FILENO, stdinbuffer, MAX_STDIN_INPUT);
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

int handle_io_client()
{
      if(serverconn->registered == NOT_REGISTERED)
      {
            log_info("[EXCEPTION WHILE MAKING CONNECTION][CONNECTION NOT REGISTERED");
            deregisterServer();
            return FAILURE;
      }

      switch(serverconn->stage)
      {
            case MESSAGE_TYPE__clienthello:
            {
                  IRCMessage *message = (IRCMessage *) calloc(1, sizeof(IRCMessage));
                  ircmessage__init(message);
                  message->dfhkey = (char *) createDFHKey(serverconn->randomkey);
                  serverconn->stage = MESSAGE_TYPE__serverhello;
                  wrapConnection(serverconn, message);

                  if(writeconnection(serverconn) == FAILURE)
                  {
                        deregisterServer();
                        exit(1);
                  }

                  break;
            }      

            case MESSAGE_TYPE__serverhello: 
            {
                  if(readconnection(serverconn, MESSAGE_TYPE__keyexchange) == SUCCESS)
                  {
                        serverconn->oppdfhkey = (uint8_t *) serverconn->payload->data->dfhkey;
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
                  msg->sharedkey =(char *) serverconn->sharedkey;
                  
                  serverconn->secure = SECURE;
                  serverconn->aeswrapper = ini_aes256_wrapper(serverconn->sharedkey);
                  serverconn->stage = MESSAGE_TYPE__unknownstage;
                  wrapConnection(serverconn, msg);

                  if(writeconnection(serverconn) == SUCCESS)
                  {
                        log_debug("[KEY EXCHANGE SWITCH CASE HANDLED]");
                  }
                  else 
                  {
                        log_error("[HANDSHAKE EXCEPTION][ERROR WHILE KEY EXCHANGE]");
                        deregisterServer();
                  }

                  break;
            }

            case MESSAGE_TYPE__unknownstage:
                  break;

            default:
                  break;
      }
      
      return SUCCESS;
}