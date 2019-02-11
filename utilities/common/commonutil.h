#ifndef COMMONUTIL_H
#define COMMONUTIL_H
#include<stdlib.h>
#include<stdio.h>  
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<signal.h>
#include<time.h>

#include "../../protobufs/payload.pb-c.h"

#define URANDOM_FILE "/dev/urandom"

#define TRUE 1
#define FALSE 0
#define PORT 4321

#define MAX_STDIN_INPUT 1024
#define MAX_DATA_LENGTH 1024
#define SUCCESS 1
#define FAILURE -1

#define SOCKET_FAMILY AF_INET
#define SOCKET_TYPE SOCK_STREAM
#define PROTOCOL IPPROTO_TCP

#define RANDOMLEN 61
#define KEYLENGTH 64
#define SIDLENGTH 16 + 1 // 16 Digits + 1 for Null

#define DFH_G 2
#define DFH_P 57
#define DFHLIMIT 10

#define NO_FD -1
#define REGISTERED 1
#define NOT_REGISTERED -1
#define SECURE 1
#define NOT_SECURE -1
#define WRITABLE 1
#define NOT_WRITABLE -1
#define UNKNOWN_STAGE -1

#define DFH(a, b) fmod(pow(a, b), DFH_P)


typedef struct {
    uint8_t *sharedkey;
    uint8_t *randomkey;
    uint8_t *oppdfhkey;
    char *ip;
    int port;
    char *sid;
    int fd;
    int id;
    int registered;
    int secure;
    int writable;
    size_t len;
    IRCPayload *payload;
    MessageType stage;
} Connection;

static FILE *urandom;

int createSocket();
char *createSessionId();
uint8_t *createRandomKey();
uint8_t *createDFHKey(uint8_t *key);
int readconnection(Connection *c, MessageType mtype);
int writeconnection(Connection *c, MessageType mtype);
void wrapConnection(Connection *c, IRCMessage *data);
int bindsocket(int fd, struct sockaddr_in address_in);
uint8_t *resolveDFHKey(uint8_t *secretkey, uint8_t *publickey);
void extract_addr_info(struct sockaddr_in clientaddr, char *ip, int *port);
int generateRandom();
void initializeCommonUtils();
void printKey(uint8_t *key);

#endif