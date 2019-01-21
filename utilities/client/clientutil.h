#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<signal.h>

#define REMOTE_SERVER_IP "0.0.0.0"
int clientfd;

struct sockaddr_in *getremoteserveraddr();
void connect_to_server(int clientfd, struct sockaddr_in addr);
int preparefds_client(int clientfd, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds);
void initiateIRCClient();