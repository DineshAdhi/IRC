#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<string.h>
#include<signal.h>

#include"../../include/serverutil.h"
#include"../../include/log.h"
#include "../../include/commonutil.h"

void terminateServer()
{
        int i;

        log_info("[IRCSERVER][SIGINT/SIGTSTP RECEIVED][HALTING SERVER]");

        for(i=0; i<CLIENT_MAX; i++)
        {
                if(conns[i].fd == NO_FD)
                        continue;
                
                close(conns[i].fd);
        }
        
        close(serverfd);
        exit(1);
}

void initializeIRCServer()
{
        int i;
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

        conns = (Connection *) calloc(CLIENT_MAX, sizeof(Connection));

        for(i=0; i<CLIENT_MAX; i++)
        {
                conns[i].fd = NO_FD;
                conns[i].id = i;
                conns[i].port = -1;
                conns[i].ip = NULL;
                conns[i].payload = NULL;
                conns[i].writable = NOT_WRITABLE;
                conns[i].stage = UNKNOWN_STAGE;
                conns[i].registered = NOT_REGISTERED;
        }

        initializeCommonUtils();
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

int preparefds_server(int serverfd, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds)
{
        FD_ZERO(read_fds);
        FD_ZERO(write_fds);
        FD_ZERO(except_fds);

        FD_SET(serverfd, read_fds);
        FD_SET(STDIN_FILENO, read_fds);

        int i, maxfd = serverfd, tempfd;

        for(i=0; i<CLIENT_MAX; i++)
        {
                tempfd = conns[i].fd;

                if(tempfd == NO_FD)
                {
                        continue;
                }

                if(tempfd > 0)
                {
                        FD_SET(tempfd, read_fds);
                        
                        if(conns[i].writable == WRITABLE)
                        {
                                FD_SET(tempfd, write_fds);
                        }
                }

                if(tempfd > maxfd)
                {
                        maxfd = tempfd;
                }
        }

        return maxfd + 1;
}

int registerClient(char *ip, int port, int remotefd)
{
        int i;

        for(i=0; i<CLIENT_MAX; i++)
        {
                if(conns[i].fd == NO_FD)
                {
                        break;
                }
        }

        if(i >= CLIENT_MAX)
        {
                log_info("[IRCCLIENT LIST OVERFLOW][DROPPING CONNECTION DUE TO MAX CONNECTION EXCEED]");
                close(remotefd);
                return FAILURE;
        }

        conns[i].ip = ip;
        conns[i].port = port;
        conns[i].fd = remotefd;
        conns[i].registered = REGISTERED;
        conns[i].payload = (IRCPayload *) calloc(1, sizeof(IRCPayload));
        conns[i].stage = MESSAGE_TYPE__clienthello;
        conns[i].secure = NOT_SECURE;
        conns[i].handshakedone = HANDSHAKE_NOT_DONE;
        conns[i].authdone = UNAUTHENTICATED;
        conns[i].sid = createSessionId();
        conns[i].randomkey = createRandomKey();

        log_info("[%s][NEW INCOMING CONNECTION][IRCCLIENT REGISTERED][ASSIGNED FD - %d][IP - %s][PORT - %d]", conns[i].sid, remotefd, ip, port);

        return i;
}

void deregisterClient(Connection *c)
{
        log_info("[%s][DEREGISTERING CONNECTION][FD - %d][IP - %s][PORT - %d]", c->sid, c->fd, c->ip, c->port);
        close(c->fd);
        c->fd = NO_FD;
}

int verifySharedKey(Connection *c)
{
        int i;

        for(i=0; i<KEYLENGTH; i++)
        {
                if(c->sharedkey[i] != c->payload->data->sharedkey[i])
                {
                        return FAILURE;
                }
        }

        return SUCCESS;
}


