#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<string.h>
#include<signal.h>

#include"serverutil.h"
#include"../logger/log.h"
#include "../common/commonutil.h"

void terminateServer()
{
        log_info("[IRCSERVER][SIGINT/SIGTSTP RECEIVED][HALTING SERVER]");
        close(serverfd);
        exit(1);
}

void initializeIRCServer()
{
        struct sigaction action;
        action.sa_handler = terminateServer;

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

        client = (int *) calloc(CLIENT_MAX, sizeof(int));
}

struct sockaddr_in* getserversockAddr()
{
        struct sockaddr_in *addr = (struct sockaddr_in *) calloc(1, sizeof(struct sockaddr_in));
        addr->sin_family = SOCKET_FAMILY;
        addr->sin_port = htons(PORT);
        addr->sin_addr.s_addr = inet_addr(SERVER_IP);

        return addr;
}

int listenforconnections(int fd)
{
        int result = listen(fd, MAX_CLIENT_BACKLOG);

        if(result < 0)
        {
                perror("listen()");
                exit(1);
        }

        return result;
}

int preparefds_server(int serverfd, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds, int *client)
{
        FD_ZERO(read_fds);
        FD_ZERO(write_fds);
        FD_ZERO(except_fds);

        FD_SET(serverfd, read_fds);
        FD_SET(STDIN_FILENO, read_fds);

        int i, maxfd = serverfd, tempfd;

        for(i=0; i<CLIENT_MAX; i++)
        {
                tempfd = client[i];

                if(tempfd > 0)
                {
                        FD_SET(tempfd, read_fds);
                        FD_SET(tempfd, write_fds);
                }

                if(tempfd > maxfd)
                {
                        maxfd = tempfd;
                }
        }

        return maxfd + 1;
}


