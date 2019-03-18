#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>

#include "../../include/log.h"
#include "../../include/serverhandler.h"
#include "../../include/serverutil.h"
#include"../../include/aes256.h"
#include "../../include/commonutil.h"
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

void handle_io_server_handshake(Connection *c)
{
        switch(c->stage)
        {
                case MESSAGE_TYPE__clienthello:
                {
                        if(readconnection(c, MESSAGE_TYPE__serverhello) == SUCCESS)
                        {
                                c->oppdfhkey = (uint8_t *) c->payload->data->key;
                                c->sharedkey = resolveDFHKey(c->randomkey, c->oppdfhkey);
                                log_debug("[%s][SERVER SHARED KEY]", c->sid); //printKey(c->sharedkey, KEYLENGTH);
                                c->stage = MESSAGE_TYPE__serverhello;
                                c->writable = WRITABLE;
                        }
                        else 
                        {
                                deregisterClient(c);
                        }

                        break;
                }
                
                case MESSAGE_TYPE__serverhello: 
                {
                        IRCMessage *ircmessage = (IRCMessage *) calloc(1, sizeof(IRCMessage));
                        ircmessage__init(ircmessage);
                        ircmessage->key = (char *)createDFHKey(c->randomkey);
                        c->stage = MESSAGE_TYPE__keyexchange;

                        wrapConnection(c, ircmessage);

                        if(writeconnection(c) != SUCCESS)
                        {
                                deregisterClient(c);
                        }

                        break;
                }

                case MESSAGE_TYPE__keyexchange: 
                {
                        c->aeswrapper = init_aes256_wrapper(c->sharedkey);
                        c->secure = SECURE;
                        c->stage = MESSAGE_TYPE__handshakedone;

                        if(readconnection(c, MESSAGE_TYPE__handshakedone) == SUCCESS)
                        {
                                if(verifySharedKey(c) == SUCCESS)
                                {
                                        log_debug("[%s][SHARED KEY VERIFICATION SUCCESS]", c->sid);
                                        c->writable = WRITABLE;
                                }
                                else 
                                {
                                        log_debug("[%s][SHARED KEY VERIFICATION FAILED]", c->sid);
                                        deregisterClient(c);
                                }
                                
                                //printKey((uint8_t *)c->payload->data->key, KEYLENGTH);
                        }
                        else 
                        {
                                log_error("[%s][ERROR READING CONNECTION DURING KEY EXCHANGE]", c->sid);
                                deregisterClient(c);
                        }

                        break;
                }

                case MESSAGE_TYPE__handshakedone:
                {
                        c->securekey = createAESKey();
                        
                        IRCMessage *ircmessage = (IRCMessage *) calloc(1, sizeof(IRCMessage));
                        ircmessage__init(ircmessage);
                        ircmessage->key = (char *) c->securekey;
                        c->stage = MESSAGE_TYPE__unknownstage;

                        wrapConnection(c, ircmessage);

                        if(writeconnection(c) == SUCCESS)
                        {
                                c->aeswrapper = init_aes256_wrapper((uint8_t *)c->securekey);
                                c->handshakedone = HANDSHAKE_DONE;
                                log_debug("[SHARED MASTER SECRET]");
                                //printKey(c->securekey, KEYLENGTH);
                        }
                        else 
                        {
                                log_error("[%s][ERROR WHILE WRITING TO CONNECTION]", c->sid);  
                                deregisterClient(c);
                        }

                        break;
                }

                case MESSAGE_TYPE__unknownstage:
                        log_info("[%s][CLIENT DISCONNECTED][FD - %d][ID - %d]", c->sid, c->fd, c->id);
                        deregisterClient(c);
                        break;

                default: 
                        log_info("[%s][HANDSHAKE EXCEPTION][UNKNOWN STAGE]", c->fd);
                        break;
        }
}

void handle_io_server_auth(Connection *c)
{

}

void handle_io_server(int id, int cfd)
{
        Connection *c = &conns[id];

        if(c->handshakedone == HANDSHAKE_NOT_DONE)
        {
                handle_io_server_handshake(c);
                return;
        }

        if(c->writable == WRITABLE)
        {
                if(writeconnection(c) == FAILURE)
                {
                        log_error("[%s][ERROR WHILE WRITING TO CONNECTION]");
                }
                return;
        }

        if(readconnection(c, MESSAGE_TYPE__unknownstage) == FAILURE)
        {
                deregisterClient(c);
                return;
        }
        

        IRCPayload *pload = c->payload;
        IRCMessage *msg = pload->data;

        switch(pload->mtype)
        {
                case MESSAGE_TYPE__auth:
                {
                        log_info("[RECEIVED REQUEST FOR AUTH]");
                        log_info("[USER ID : %s][PASS : %s]", msg->userconfig->id, msg->userconfig->password);
                        break;
                }

                case MESSAGE_TYPE__signup:
                {
                        log_info("[RECIEVED SIGN UP CALL]");
                        log_info("[SIGN UP CALL][USER ID : %s][PASS : %s]", msg->userconfig->id, msg->userconfig->password);
                        break;
                }

                default:
                   log_error("[%s][MTYPE UNKNOWN]", c->sid);
                   deregisterClient(c);
                   break;
        }
}