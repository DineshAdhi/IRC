../protobuftest/#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<unistd.h>

#include"utilities/netutil.h"
#include"utilities/log.c"

int main()
{
        struct sockaddr_in *addr = getserversockAddr();
        int serverfd = createSocket();
        int maxfd = serverfd;
        int client[CLIENT_MAX];
        fd_set read_fds, write_fds, except_fds;
        
        bindsocket(serverfd, *addr); 
        listenforconnections(serverfd);

        log_info("[IRCSERVER][LISTEING TO - %s][PORT - %d]", SERVER_IP, PORT);
       
        while(TRUE)
        {
    
                maxfd = preparefds(serverfd, &read_fds, &write_fds, &except_fds, client);

                int activity = select(maxfd + 1, &read_fds, &write_fds, &except_fds, NULL);

                switch(activity)
                {
                       case -1:
                           perror("select()");
                           break;
                       
                       case 0:
                            perror("select()");
                            break;

                        default:

                            if(FD_ISSET(STDIN_FILENO, &read_fds))
                            {
                                   char *buffer = (char *) calloc(MAX_STDIN_INPUT, sizeof(char));
                                   int bytes = read(STDIN_FILENO, buffer, MAX_STDIN_INPUT);
                                   
                                   log_info("STDIN  : %s",buffer);
                            }
                }
        }
}
