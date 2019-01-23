#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<signal.h>

#define TRUE 1
#define FALSE 0
#define PORT 4321

#define MAX_STDIN_INPUT 1024
#define MAX_DATA_LENGTH 1024
#define SUCCESS 1
#define FAILURE -1

#define SOCKET_FAMILY AF_INET
#define SOCKET_TYPE SOCK_STREAM
#define PROTOCOL IPPROTO_TCP

int createSocket();
int bindsocket(int fd, struct sockaddr_in address_in);
void extract_client_info(struct sockaddr_in clientaddr, char *ip, int *port);