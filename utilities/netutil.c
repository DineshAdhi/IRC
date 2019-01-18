#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<string.h>

#include"netutil.h"

struct sockaddr_in* getserversockAddr()
{
        struct sockaddr_in *addr = (struct sockaddr_in *) calloc(1, sizeof(struct sockaddr_in));
        addr->sin_family = SOCKET_FAMILY;
        addr->sin_port = htons(PORT);
        addr->sin_addr.s_addr = inet_addr(SERVER_IP);

        return addr;
}

int createSocket()
{
        int fd = socket(SOCKET_FAMILY, SOCKET_TYPE, PROTOCOL);

        if(fd < 0)
        {
                perror("socket()");
                exit(1);
        }

        return fd;
}

int bindsocket(int fd, struct sockaddr_in address_in)
{
        socklen_t addr_size = sizeof(address_in);
        struct sockaddr *addr = (struct sockaddr *) &address_in;

        int result = bind(fd, addr, addr_size);
        
        if(result < 0)
        {
                perror("bind()");
                exit(1);
        }

        return result;
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

int preparefds(int serverfd, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds, int *client)
{
        FD_ZERO(read_fds);
        FD_ZERO(write_fds);
        FD_ZERO(except_fds);

        FD_SET(serverfd, read_fds);
        //FD_SET(serverfd, write_fds);
        FD_SET(STDIN_FILENO, read_fds);

        client = (int *) calloc(CLIENT_MAX, sizeof(int));

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

void extract_client_info(struct sockaddr_in clientaddr, char *ip, int *port)
{
        *port = ntohs(clientaddr.sin_port);
        strcpy(ip, inet_ntoa(clientaddr.sin_addr));
}
