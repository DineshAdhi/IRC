#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>

#include "../logger/log.h"
#include "serverhandler.h"
#include "serverutil.h"
#include"../crypto/aes256.h"
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

        extract_addr_info(clientaddr, clientip, &port);
        registerClient(clientip, port, remotefd);

        return remotefd;
}

void handle_io_server(int id, int cfd)
{
        Connection *c = &conns[id];

        switch(c->stage)
        {
                case MESSAGE_TYPE__clienthello:
                {
                        if(readconnection(c, MESSAGE_TYPE__serverhello) == SUCCESS)
                        {
                                c->oppdfhkey = (uint8_t *) c->payload->data->dfhkey;
                                c->sharedkey = resolveDFHKey(c->randomkey, c->oppdfhkey);
                                log_debug("[%s][SERVER SHARED KEY]", c->sid); printKey(c->sharedkey, KEYLENGTH);
                                c->stage = MESSAGE_TYPE__serverhello;
                                c->writable = WRITABLE;
                                break;
                        }
                        else 
                        {
                                log_error("[%s][ERROR WHILE READING FROM CONNECTION]", c->sid);
                                deregisterClient(c);
                        }
                }
                
                case MESSAGE_TYPE__serverhello: 
                {
                        IRCMessage *ircmessage = (IRCMessage *) calloc(1, sizeof(IRCMessage));
                        ircmessage__init(ircmessage);
                        ircmessage->dfhkey = (char *)createDFHKey(c->randomkey);
                        c->stage = MESSAGE_TYPE__keyexchange;

                        wrapConnection(c, ircmessage);

                        if(writeconnection(c) == SUCCESS)
                        {
                                
                        }
                        else 
                        {
                                log_error("[%s][ERROR WHILE WRITING TO CONNECTION]", c->sid);
                                deregisterClient(c);
                        }

                        break;
                }

                case MESSAGE_TYPE__keyexchange: 
                {
                        c->aeswrapper = ini_aes256_wrapper(c->sharedkey);
                        c->secure = SECURE;
                        c->stage = MESSAGE_TYPE__unknownstage;

                        if(readconnection(c, MESSAGE_TYPE__unknownstage) == SUCCESS)
                        {
                                log_debug("READ KEY EXCHANGE SUCCESSFULLY");
                        }
                        else 
                        {
                                log_error("[%s][ERROR DURING KEY EXCHANGE]", c->sid);
                                deregisterClient(c);
                        }

                        break;
                }

                case MESSAGE_TYPE__unknownstage:
                        break;

                default: 
                        //log_info("[%s][HANDSHAKE EXCEPTION][UNKNOWN STAGE]", c->fd);
                        break;
        }
}