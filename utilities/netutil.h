#ifndef NETUTIL_H
#define NETUTIL_H
#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>

#include "models.h"

#define TRUE 1
#define FALSE 0
#define PORT 4321
#define SERVER_IP "0.0.0.0"
#define CLIENT_MAX 100000
#define MAX_CLIENT_BACKLOG 1000
#define MAX_STDIN_INPUT 1024
#define MAX_PAYLOAD_SIZE 1024
#define NO_FD -1

#define SOCKET_FAMILY AF_INET
#define SOCKET_TYPE SOCK_STREAM
#define PROTOCOL IPPROTO_TCP


struct sockaddr_in* getserversockAddr();
int createSocket();
int bindsocket(int fd, struct sockaddr_in address_in);
int listenforconnections(int fd);
int preparefds(int serverfd, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds, client_model *clients);
void extract_client_info(struct sockaddr_in clientaddr, char *ip, int *port);
void initialize_clients(client_model *clients);
#endif
