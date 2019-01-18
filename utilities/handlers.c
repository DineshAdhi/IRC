#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>

#include "log.h"
#include "handlers.h"
#include "netutil.h"
#include "models.h"

int handle_incoming_connection(int serverfd, client_model *remoteclient)
{
        struct sockaddr_in clientaddr;
        socklen_t clientaddr_size = sizeof(clientaddr);
        memset(&clientaddr, 0, sizeof(clientaddr));

        int remotefd = accept(serverfd, (struct sockaddr *) &clientaddr, &clientaddr_size);

        if(remotefd < 0)
        {
                perror("accpet()");
                log_error("[ERROR WHILE ACCEPTING INCOMING CONNECTION][FD - %d]", remotefd);
                return -1;
        }

        char *clientip = (char *) calloc(100, sizeof(char));
        int port;

        extract_client_info(clientaddr, clientip, &port);

        remoteclient->ip = clientip;
        remoteclient->port = port;
        remoteclient->sockaddr = clientaddr;
        remoteclient->fd = remotefd;

        log_info("[INCOMING CONNECTION][ASSIGNED FD - %d][IP - %s][PORT - %d]", remotefd, clientip, port);

        return remotefd;
}
