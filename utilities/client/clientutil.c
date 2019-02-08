#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<signal.h>
#include<netdb.h>

#include"../common/commonutil.h"
#include"../logger/log.h"
#include"clientutil.h"

void terminateClient()
{
        log_info("[IRCCLIENT][SIGINT/SIGTSTP RECEIVED][HALTING SERVER]");
        close(serverconn->fd);
        exit(1);
}

void initiateIRCClient()
{
        struct sigaction action;
        action.sa_handler = terminateClient;

        if(sigaction(SIGINT, &action, 0) < 0)
        {
                perror("sigaction");
                log_warn("[SIGACTION EXCEPTION][SIGNT]");
        }

        if(sigaction(SIGTSTP, &action, 0) < 0)
        {
                perror("sigaction");
                log_warn("[SIGACTION EXCEPTION][SIGTSTP]");
        }

        serverconn = (Connection *) calloc(1, sizeof(Connection));
        serverconn->fd = NO_FD;
        serverconn->port = -1;
        serverconn->ip = NULL;
        serverconn->payload = NULL;
        serverconn->writable = NOT_WRITABLE;
        serverconn->stage = UNKNOWN_STAGE;
        serverconn->registered = NOT_REGISTERED;

        initializeCommonUtils();
}

struct sockaddr_in getremoteserveraddr()
{
         struct hostent *host = (struct hostent *) calloc(0, sizeof(struct hostent));
         host = gethostbyname(REMOTE_SERVER_DOMAIN);

         struct sockaddr_in *addr = (struct sockaddr_in *) calloc(1, sizeof(struct sockaddr_in));
         addr->sin_family = SOCKET_FAMILY;
         addr->sin_port = htons(PORT);
         //addr->sin_addr.s_addr = inet_addr(REMOTE_SERVER_IP);
         addr->sin_addr.s_addr = *((int *)host->h_addr_list[0]);

         return *addr;
}


int _connect_to_server(int reconnect)
{
      serverconn->fd = createSocket();

      struct sockaddr_in addr = getremoteserveraddr();

      int result = connect(serverconn->fd, (struct sockaddr *)&addr, sizeof(addr));
      
      if(result < 0)
      {
            if(reconnect == !RECONNECT)
            {
                  log_fatal("[IRCCLIENT][FATAL][CONNECTION REFUSED]");
                  perror("connect()");
                  exit(1);
            }
            else
            {
                  return FAILURE;
            }
      }

      char ip[16] = {};
      int port;

      extract_addr_info(addr, ip, &port);
      
      serverconn->ip = ip;
      serverconn->port = port;
      serverconn->registered = REGISTERED;
      serverconn->payload = (IRCPayload *) calloc(1, sizeof(IRCPayload));
      serverconn->stage = MESSAGE_TYPE__clienthello;
      serverconn->secure = NOT_SECURE;
      serverconn->writable = WRITABLE;
      serverconn->randomkey = createRandomKey();

      log_info("[IRCCLIENT][CONNECTED TO SERVER][REMOTE SERVER IP - %s][PORT - %d]", serverconn->ip, serverconn->port);

      return SUCCESS;
}

void preparefds_client(fd_set *read_fds, fd_set *write_fds, fd_set *except_fds)
{
      FD_ZERO(read_fds);
      FD_ZERO(write_fds);
      FD_ZERO(except_fds);

      FD_SET(serverconn->fd, read_fds);
      
      if(serverconn->writable == WRITABLE)
      {
            FD_SET(serverconn->fd, write_fds);
      }

      FD_SET(STDIN_FILENO, read_fds);
}

void deregisterServer()
{
      log_info("[DEREGISTERING SERVER]");
      close(serverconn->fd);
}