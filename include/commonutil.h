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

#include "../protobufs/payload.pb-c.h"

#define printPrompt() {printf(">> "); fflush(stdout);}

#define URANDOM_FILE "/dev/urandom"

#define TRUE 1
#define FALSE 0
#define PORT 4321

#define MAX_STDIN_INPUT 1024
#define MAX_DATA_LENGTH 10240
#define SUCCESS 1
#define FAILURE -1

#define SOCKET_FAMILY AF_INET
#define SOCKET_TYPE SOCK_STREAM
#define PROTOCOL IPPROTO_TCP

#define RANDOMLEN 61
#define KEYLENGTH 32
#define SIDLENGTH 16 

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
#define HANDSHAKE_DONE 1
#define HANDSHAKE_NOT_DONE -1
#define AUTHENTICATED 1
#define UNAUTHENTICATED -1
#define READ_FAILURE 2
#define REQUIRED 1
#define NOT_REQUIRED -1

#define SERVER_LOGFILE_PATH "logs/serverlogs"
#define CLIENT_LOGFILE_PATH "logs/clientlogs"
#define USERCONFIG_FILE_PATH "config/config.irc"
#define DATABAST_FILE_PATH "bin/database.db"

//////////////////////////////  AES Variables

#define AESROUNDKEYLEN 240 

#define DFH(a, b) fmod(pow(a, b), DFH_P)

typedef struct {
    uint8_t rkey[AESROUNDKEYLEN];
}AES_CTX;


typedef struct {
    AES_CTX *ctx;
    uint8_t *plain;
    size_t length;
    uint8_t *hash;
    char *hex;
    char *base64;
} AES_WRAPPER;

////////////////////////////


typedef struct {
    uint8_t *sharedkey;
    uint8_t *randomkey;
    uint8_t *oppdfhkey;
    uint8_t *securekey;
    AES_WRAPPER *aeswrapper;
    char *ip;
    int port;
    char *sid;
    int fd;
    int id;
    int registered;
    int secure;
    int writable;
    int handshakedone;
    int authdone;
    uint8_t *buffer;
    size_t len;
    IRCPayload *payload;
    MessageType stage;
} Connection;


static FILE *urandom;

int createSocket();
char *createSessionId();
uint8_t *createRandomKey();
uint8_t *createAESKey();
uint8_t *createDFHKey(uint8_t *key);
int readconnection(Connection *c, MessageType mtype);
int writeconnection(Connection *c);
void wrapConnection(Connection *c, IRCMessage *data);
int bindsocket(int fd, struct sockaddr_in address_in);
uint8_t *resolveDFHKey(uint8_t *secretkey, uint8_t *publickey);
void extract_addr_info(struct sockaddr_in clientaddr, char *ip, int *port);
int generateRandom();
void initializeCommonUtils();
char* printKey(uint8_t *key, int len);
char* readFromStdin();

#endif
