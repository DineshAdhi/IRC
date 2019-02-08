#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>

#include "../logger/log.h"
#include "serverhandler.h"
#include "serverutil.h"
#include "../common/commonutil.h"
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
        registerClient(clientip, port, remotefd);

        return remotefd;
}

void handle_io(int id, int cfd)
{
        Connection *c = &conns[id];

        // if(c->stage != c->payload->mtype)
        // {
        //         log_error("[%s][EXCEPTION DURING HANDSHAKE][STAGE MISMATCH]", conns[id].sid);
        //         deregisterClient(c);
        //         return;
        // }

        switch(c->stage)
        {
                log_debug("ENTERED SWITCH");
                case MESSAGE_TYPE__clienthello:
                {
                        if(readconnection(c, MESSAGE_TYPE__clienthello) == SUCCESS)
                        {
                                c->oppdfhkey = (uint8_t *) c->payload->data->dfhkey;
                                c->sharedkey = resolveDFHKey(c->randomkey, c->oppdfhkey);
                                c->stage = MESSAGE_TYPE__serverhello;
                                
                                IRCMessage *ircmessage = (IRCMessage *) malloc(1);
                                ircmessage->dfhkey = (char *)createDFHKey(c->randomkey);

                                wrapConnection(c, ircmessage);
                                c->writable = WRITABLE;
                                break;
                        }
                }
                
                case MESSAGE_TYPE__serverhello: 
                {
                        break;
                }

                default: 
                        break;
        }
}