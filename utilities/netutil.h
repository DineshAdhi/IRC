#include<stdlib.h>
#include <stdio.h>  
#include <arpa/inet.h>    //close  
#include <sys/types.h>  
#include <sys/socket.h>  

#define TRUE 1
#define FALSE 0
#define PORT 4321
#define SERVER_IP "0.0.0.0"
#define CLIENT_MAX 100000
#define SOCKET_FAMILY AF_INET
#define SOCKET_TYPE SOCK_STREAM
#define PROTOCOL IPPROTO_TCP

struct sockaddr_in* getserversockAddr()
{
        struct sockaddr_in *addr = (struct sockaddr_in *) calloc(1, sizeof(struct sockaddr_in));
        addr->sin_family = SOCKET_FAMILY;
        addr->sin_port = PORT;
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
