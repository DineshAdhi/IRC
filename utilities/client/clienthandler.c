#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<signal.h>

#include"../common/commonutil.h"
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
                  message->dfhkey = (char *) createDFHKey(serverconn->randomkey);

                  serverconn->stage = MESSAGE_TYPE__serverhello;
                  wrapConnection(serverconn, message);

                  if(writeconnection(serverconn, MESSAGE_TYPE__serverhello) == FAILURE)
                  {
                        deregisterServer();
                        exit(1);
                  }

                  break;
            }      

            default:
                  log_info("[UNKNOWN STAGE EXCEPTION]");
                  break;
      }
      
      return SUCCESS;
}