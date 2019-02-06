#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<signal.h>

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

#define DFH_G 2
#define DFH_P 57

#define DFH(a, b) fmod(pow(a, b), DFH_P)
#define GENERATE_RANDOM() ({ srand(time(0)); rand() % RANDOMLEN + 1; })

int createSocket();
uint8_t *createRandomKey();
uint8_t *createDFHKey(uint8_t *key);
uint8_t *resolveDFHKey(uint8_t *secretkey, uint8_t *publickey);
int bindsocket(int fd, struct sockaddr_in address_in);
void extract_client_info(struct sockaddr_in clientaddr, char *ip, int *port);