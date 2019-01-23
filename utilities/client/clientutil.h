#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<signal.h>

#define REMOTE_SERVER_IP "127.0.0.1"
#define REMOTE_SERVER_DOMAIN "localhost"
#define RECONNECT 1
#define MAX_CONNECT_RETRY 10

int clientfd;

#define initiate_connect_to_server() _connect_to_server(!RECONNECT) 
#define connect_to_server(reconnect) _connect_to_server(reconnect) 

struct sockaddr_in getremoteserveraddr();
int _connect_to_server(int reconnect);
int preparefds_client(int clientfd, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds);
void initiateIRCClient();