#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<signal.h>

#include"../common/commonutil.h"
#include"../logger/log.h"
#include"clientutil.h"

void terminateClient()
{

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
}

struct sockaddr_in *getremoteserveraddr()
{
         struct sockaddr_in *addr = (struct sockaddr_in *) calloc(1, sizeof(struct sockaddr_in));
         addr->sin_family = SOCKET_FAMILY;
         addr->sin_port = htons(PORT);
         addr->sin_addr.s_addr = inet_addr(REMOTE_SERVER_IP);

         return addr;
}

void connect_to_server(int clientfd, struct sockaddr_in addr)
{
      int result = connect(clientfd, (struct sockaddr *)&addr, sizeof(addr));
      if(result < 0)
      {
            log_fatal("[IRCCLIENT][FATAL][CONNECTION REFUSED]");
            perror("connect()");
            exit(1);
      }

      char serverip[16] = {};
      int port;

      extract_client_info(addr, serverip, &port);

      log_info("[IRCCLIENT][CONNECTED TO SERVER][REMOTE SERVER IP - %s][PORT - %d]", serverip, port);
}

int preparefds_client(int clientfd, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds)
{
      FD_ZERO(read_fds);
      FD_ZERO(write_fds);
      FD_ZERO(except_fds);

      FD_SET(clientfd, read_fds);
      FD_SET(STDIN_FILENO, read_fds);

      return clientfd + 1;
}
