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

int read_from_server()
{
      char *buffer = (char *) calloc(clientfd, MAX_DATA_LENGTH);
      int bytes = read(clientfd, buffer, MAX_DATA_LENGTH);

      if(bytes == 0 && initiateReconnect() == FAILURE)
      {
            log_fatal("[IRCCLIENT][SERVER DISCONNECTED]");
            close(clientfd);
            return -1;
      }

      log_debug("[LENGTH - %d]", bytes);

      return bytes;
}
