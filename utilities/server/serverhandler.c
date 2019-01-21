#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>

#include "../logger/log.h"
#include "serverhandler.h"
#include "serverutil.h"
#include "../../protobufs/payload.pb-c.h"

int handle_incoming_connection(int serverfd)
{
        struct sockaddr_in clientaddr;
        socklen_t clientaddr_size = sizeof(clientaddr);
        int remotefd = -1, i;

        memset(&clientaddr, 0, sizeof(clientaddr));

        remotefd = accept(serverfd, (struct sockaddr *) &clientaddr, &clientaddr_size);

        if(remotefd < 0)
        {
                perror("accpet()");
                log_error("[ERROR WHILE ACCEPTING INCOMING CONNECTION][FD - %d]", remotefd);
                return -1;
        }

        char *clientip = (char *) calloc(100, sizeof(char));
        int port;

        extract_client_info(clientaddr, clientip, &port);

        log_info("[INCOMING CONNECTION][ASSIGNED FD - %d][IP - %s][PORT - %d]", remotefd, clientip, port);
        
        return remotefd;
}

void handle_data_from_stdin()
{
        char *stdinbuffer = (char *) calloc(MAX_STDIN_INPUT, sizeof(char));
        int bytes = read(STDIN_FILENO, stdinbuffer, MAX_STDIN_INPUT);


        Payload pload = PAYLOAD__INIT;
        pload.mtype = MESSAGE_TYPE__MSG_TYPE_SERVER_HELLO;
        
        SERVERHELLO shello = SERVERHELLO__INIT;
        shello.ip = stdinbuffer;
        shello.port = 4321;

        pload.serverhello = &shello;

        int len = payload__get_packed_size(&pload);

        uint8_t buffer[len];

        payload__pack(&pload, buffer);
        
        write(serverfd+1, buffer, len);
}
