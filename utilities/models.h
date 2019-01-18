#ifndef MODELS_H
#define MODELS_H
typedef struct 
{
        int port;
        int fd;
        char *ip;
        char *sessionid;
        struct sockaddr_in sockaddr;
} client_model;
#endif
