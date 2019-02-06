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

#include "commonutil.h"
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