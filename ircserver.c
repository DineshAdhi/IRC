#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<unistd.h>

#include"include/serverutil.h"
#include"include/serverhandler.h"
#include"include/log.h"
#include"include/commonutil.h"

int main()
{
        initializeIRCServer();
        struct sockaddr_in *addr = getserversockAddr();
        serverfd = createSocket();
        int i, maxfd = serverfd;
        fd_set read_fds, write_fds, except_fds;
        
        bindsocket(serverfd, *addr);
        listenforconnections(serverfd);

        log_info("[IRCSERVER][LISTENING TO - %s][PORT - %d]", SERVER_IP, PORT);
       
        while(TRUE)
        {
                maxfd = preparefds_server(serverfd, &read_fds, &write_fds, &except_fds);

                int activity = select(maxfd, &read_fds, &write_fds, &except_fds, NULL);

                switch(activity)
                {
                       case -1:
                           perror("select()");
                           exit(1);
                           break;
                       
                       case 0:
                            perror("select()");
                            break;

                        default:

                            if(FD_ISSET(serverfd, &read_fds))
                            {
                                   int remotefd = handle_incoming_connection(serverfd);
                            }

                            for(i=0; i<maxfd; i++)
                            {
                                    int cfd = conns[i].fd;

                                    if(conns[i].fd == NO_FD)
                                    {
                                            continue;
                                    }

                                    if(FD_ISSET(cfd, &read_fds))
                                    {
                                            if(conns[i].registered != REGISTERED || conns[i].payload == NULL)
                                            {
                                                    deregisterClient(&conns[i]);
                                                    continue;
                                            }
                                        
                                            handle_io_server(i, cfd);
                                    }

                                    if(FD_ISSET(cfd, &write_fds))
                                    {
                                            if(conns[i].registered != REGISTERED || conns[i].payload == NULL)
                                            {
                                                    deregisterClient(&conns[i]);
                                                    continue;
                                            }

                                            handle_io_server(i, cfd);
                                    }
                            }
                }
        }
}
