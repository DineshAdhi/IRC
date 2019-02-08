#ifndef SERVERUTIL_H
#define SERVERUTIL_H
#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<signal.h>

#include"../../protobufs/payload.pb-c.h"
#include"../common/commonutil.h"

#define SERVER_IP "0.0.0.0"
#define CLIENT_MAX 1000
#define MAX_CLIENT_BACKLOG 1000

Connection *conns;

int serverfd;


void terminateServer();
void initializeIRCServer();
void deregisterClient(Connection *c);
int listenforconnections(int fd);
struct sockaddr_in* getserversockAddr();
int registerClient(char *ip, int port, int remotefd);
int preparefds_server(int serverfd, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds);



#endif
