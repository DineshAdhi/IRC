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

int read_from_server()
{
      char *buffer = (char *) calloc(clientfd, MAX_DATA_LENGTH);
      int bytes = read(clientfd, buffer, MAX_DATA_LENGTH);

      if(bytes == 0)
      {
            log_fatal("[IRCCLIENT][SERVER DISCONNECTED]");
            close(clientfd);
            return -1;
      }

      log_debug("[LENGTH - %d]", bytes);

      return bytes;
}