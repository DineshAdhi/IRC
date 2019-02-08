#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<signal.h>
#include<string.h>
#include<math.h>
#include<time.h>

#include"../../protobufs/payload.pb-c.h"
#include "commonutil.h"
#include "../server/serverutil.h"
#include "../logger/log.h"

char RAND[63] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";


int createSocket()
{
        int fd = socket(SOCKET_FAMILY, SOCKET_TYPE, PROTOCOL);
        if(fd < 0)
        {
                perror("socket()");
                exit(1);
        }
        return fd;
}

int bindsocket(int fd, struct sockaddr_in address_in)
{
        socklen_t addr_size = sizeof(address_in);
        struct sockaddr *addr = (struct sockaddr *) &address_in;

        int result = bind(fd, addr, addr_size);
        
        if(result < 0)
        {
                perror("bind()");
                exit(1);
        }

        return result;
}

void extract_client_info(struct sockaddr_in clientaddr, char *ip, int *port)
{
        *port = ntohs(clientaddr.sin_port);
        strcpy(ip, inet_ntoa(clientaddr.sin_addr));
}

char *createSessionId()
{
        int i;

        char *sid = (char *)calloc(SIDLENGTH, sizeof(char));

        for(i=0; i<SIDLENGTH; i++)
        {
                srand(time(NULL));
                int r = rand() % 20 + 1;
                log_info("%d", r);
                sid[i] = (uint8_t) RAND[r];
        }

        return sid;  
}

uint8_t *createRandomKey()
{
        int i;

        uint8_t *key = (uint8_t *)calloc(KEYLENGTH, sizeof(uint8_t));

        for(i=0; i<KEYLENGTH; i++)
        {

                int r = GENERATE_RANDOM();
                key[i] = (uint8_t) RAND[r];
        }

        return key;
}

uint8_t *createDFHKey(uint8_t *key)
{
        int i;

        for(i=0; i<KEYLENGTH; i++)
        {
                key[i] = DFH(DFH_G, key[i]);
        }

        return key;
}

uint8_t *resolveDFHKey(uint8_t *secretkey, uint8_t *publickey)
{
        int i;

        uint8_t *rKey = (uint8_t *) calloc(KEYLENGTH, sizeof(uint8_t));

        for(i=0; i<KEYLENGTH; i++)
        {
                rKey[i] = DFH(publickey[i], secretkey[i]);
        }

        return rKey;
}

int readconnection(Connection *c, MessageType mtype)
{
        uint8_t *buffer = (uint8_t *) calloc(MAX_DATA_LENGTH, sizeof(uint8_t)); 

        log_debug("%d CONNECTION READ", c->len);

        c->len = read(c->fd, buffer, MAX_DATA_LENGTH);
        c->payload = ircpayload__unpack(NULL, c->len, buffer);

        if(c->len == 0)
        {
                log_info("[%s][CLIENT DISCONNECTION]", c->sid);
                deregisterClient(c);
                return FAILURE;
        }

        if(c->secure == NOT_SECURE)
        {
                if(c->payload->mtype != mtype)
                {
                        log_error("[%s][HANDSHAKE EXCEPTION][MISMATCH OF STAGE]", c->sid, c->len);
                        deregisterClient(c);
                        return FAILURE;
                }
        }

        return SUCCESS;
}

void wrapConnection(Connection *c, IRCMessage *data)
{
        IRCPayload *payload = (IRCPayload *)calloc(1, sizeof(IRCPayload));
        payload->data = data;
        payload->mtype = c->stage;

        c->payload = payload;
}