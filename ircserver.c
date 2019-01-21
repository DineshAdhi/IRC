#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<unistd.h>

#include"utilities/server/serverutil.h"
#include"utilities/server/serverhandler.h"
#include"utilities/logger/log.h"
#include"utilities/common/commonutil.h"

int main()
{
        initializeIRCServer();
        struct sockaddr_in *addr = getserversockAddr();
        serverfd = createSocket();
        int maxfd = serverfd;
        fd_set read_fds, write_fds, except_fds;
        
        bindsocket(serverfd, *addr);
        listenforconnections(serverfd);

        log_info("[IRCSERVER][LISTEING TO - %s][PORT - %d]", SERVER_IP, PORT);
       
        while(TRUE)
        {
    
                maxfd = preparefds_server(serverfd, &read_fds, &write_fds, &except_fds, client);

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
                                   handle_data_from_stdin();
                            }

                            if(FD_ISSET(serverfd, &read_fds))
                            {
                                   int remotefd = handle_incoming_connection(serverfd);
                            }
                }
        }
}
