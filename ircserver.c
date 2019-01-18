#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<unistd.h>

#include"utilities/netutil.h"
#include"utilities/handlers.h"
#include"utilities/models.h"
#include"utilities/log.h"

client_model *clients; 

int main()
{
        struct sockaddr_in *addr = getserversockAddr();
        int serverfd = createSocket();
        int maxfd = serverfd;
        int i = 0;
        fd_set read_fds, write_fds, except_fds;
        
        bindsocket(serverfd, *addr);
        listenforconnections(serverfd);
        clients = (client_model *) calloc(CLIENT_MAX, sizeof(client_model));

        log_info("[IRCSERVER][LISTEING TO - %s][PORT - %d]", SERVER_IP, PORT);

        initialize_clients(clients);

        while(TRUE)
        {
    
                maxfd = preparefds(serverfd, &read_fds, &write_fds, &except_fds, clients);

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

                            if(FD_ISSET(serverfd, &read_fds))
                            {
                                   client_model remoteclient;
                                   memset(&remoteclient, 0, sizeof(remoteclient));
                                   int remotefd = handle_incoming_connection(serverfd, &remoteclient);

                                   for(i=0; i<CLIENT_MAX; i++)
                                   {
                                            if(clients[i].fd == NO_FD)
                                            {
                                                    //memcpy(&clients[i], &remoteclient, sizeof(remoteclient));
                                                    clients[i] = remoteclient;
                                                    break;
                                            }
                                   }
                            }
                            
                            for(i=0; i<CLIENT_MAX; i++)
                            {
                                    int fd = clients[i].fd;

                                    if(fd == NO_FD)
                                    {
                                            continue;
                                    }

                                    if(FD_ISSET(fd, &read_fds))
                                    {
                                            printf("Reading from : %d", fd);
                                            
                                            char *buffer = calloc(MAX_PAYLOAD_SIZE, sizeof(char));

                                            int bytes = read(fd, buffer, MAX_PAYLOAD_SIZE);
                                            buffer[bytes] = '\0';

                                            if(bytes == 0)
                                            {
                                                    log_warn("[CLIENT DISCONNECTED][FD - %d][IP - %s][PORT - %d]", clients[i].fd, clients[i].ip, clients[i].port);
                                                    close(clients[i].fd);
                                                    clients[i].fd = -1;
                                                    continue;
                                            }

                                            log_info("[CLIENT MESSAGE][MESSAGE - %s][LENGTH - %d", buffer, bytes);
                                    }

                                    if(FD_ISSET(fd, &except_fds))
                                    {
                                            printf("Closed : %d", fd);
                                    }
                            }
                }
        }
}
