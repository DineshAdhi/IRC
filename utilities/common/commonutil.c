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
#include<fcntl.h>

#include"../../protobufs/payload.pb-c.h"
#include "commonutil.h"
#include "../server/serverutil.h"
#include "../logger/log.h"

char RAND[63] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
char SID[10] = "1234567890";

void initializeCommonUtils()
{
        urandom = fopen(URANDOM_FILE, "rb");
}

int GENERATE_RANDOM()
{
        int r;

        do 
        {
                if( (r = fgetc(urandom)) == EOF)
                {
                        log_error("[URANDOM EXCEPTION]");
                }

        } while(r >= (UCHAR_MAX + 1)/ RANDOMLEN * RANDOMLEN);

        return r % RANDOMLEN;
}

void printKey(uint8_t *key, int len)
{
        int i;

        char *hash = (char *) calloc(len, 2);
        int itr = 0;

        for(i=0; i<len * 2; i=i+2)
        {
                printf("%02X ", key[itr++]);
                fflush(stdout);
        }

        printf("\n");
        fflush(stdout);
}

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

void extract_addr_info(struct sockaddr_in clientaddr, char *ip, int *port)
{
        *port = ntohs(clientaddr.sin_port);
        strcpy(ip, inet_ntoa(clientaddr.sin_addr));
}

char *createSessionId()
{
        int i;

        char *sid = (char *)calloc(SIDLENGTH, sizeof(char));

        for(i=0; i<SIDLENGTH-1; i++)
        {
                int r = GENERATE_RANDOM() % 10;
                sid[i] = SID[r];
        }

        sid[SIDLENGTH] = '\0';

        return sid;  
}

uint8_t *createRandomKey()
{
        int i;

        uint8_t *key = (uint8_t *)calloc(KEYLENGTH, sizeof(uint8_t));


        for(i=0; i<KEYLENGTH; i++)
        {
                int r = GENERATE_RANDOM();
                key[i] = (uint8_t) RAND[r] % DFHLIMIT;
        }
        
        return key;
}

uint8_t *createDFHKey(uint8_t *key)
{
        int i;

        uint8_t *dfhkey = (uint8_t*) calloc(KEYLENGTH, sizeof(uint8_t));

        for(i=0; i<KEYLENGTH; i++)
        {
                dfhkey[i] = DFH(DFH_G, key[i]);
        }

        return dfhkey;
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
        c->buffer = (uint8_t *) calloc(MAX_DATA_LENGTH, sizeof(uint8_t)); 

        c->len = read(c->fd, c->buffer, MAX_DATA_LENGTH);

        if(c->secure == SECURE)
        {
                c->aeswrapper->hash = c->buffer;
                c->aeswrapper->length = c->len;
                wrapper_aes256_decrypt(c->aeswrapper);
                c->buffer = c->aeswrapper->plain;
        }

        c->payload = ircpayload__unpack(NULL, c->len, c->buffer);

        if(c->payload == NULL)
        {
                log_info("[%s][EXCEPTION WHILE READING][PAYLOAD_NULL]", c->sid);
                return FAILURE;
        }


        if(c->len == 0)
        {
                log_info("[%s][CLIENT DISCONNECTION]", c->sid);
                return FAILURE;
        }

        log_info("[%s][READ %d FROM CLIENT]", c->sid, c->len);

        if(c->secure == NOT_SECURE)
        {
                if(c->payload->mtype != mtype)
                {
                        log_error("[%s][HANDSHAKE EXCEPTION][MISMATCH OF STAGE]", c->sid, c->len);
                        return FAILURE;
                }
        }

        return SUCCESS;
}

int writeconnection(Connection *c)
{
        if( write(c->fd, c->buffer, c->len) < 0 )
        {
                log_info("[EXCEPTION WHILE WRITING TO CONNECTION]");
                return FAILURE;
        }

        c->writable = NOT_WRITABLE;

        free(c->payload);
        free(c->buffer);
        
        return SUCCESS;
}

void wrapConnection(Connection *c, IRCMessage *data)
{
        c->payload = (IRCPayload *) calloc(1, sizeof(IRCPayload));
        ircpayload__init(c->payload);
        
        c->payload->data = data;
        c->payload->mtype = c->stage;

        c->len = ircpayload__get_packed_size(c->payload);
        c->buffer = (uint8_t *) calloc(c->len, sizeof(uint8_t));
        ircpayload__pack(c->payload, c->buffer);

        if(c->secure == SECURE)
        {
                c->aeswrapper->plain = c->buffer;
                c->aeswrapper->length = c->len;

                c->len = wrapper_aes256_encrypt(c->aeswrapper);

                c->buffer = c->aeswrapper->hash;
        }
}