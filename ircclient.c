#include<stdio.h>
#include<stdlib.h>
#include"utilities/server/serverutil.h"
#include"protobufs/payload.pb-c.h"

int main()
{
    int sockfd = createSocket();
    struct sockaddr_in remoteclient; 

    remoteclient.sin_family = AF_INET;
    remoteclient.sin_addr.s_addr = inet_addr("127.0.0.1");
    remoteclient.sin_port = htons(4321);

    if(connect(sockfd, (struct sockaddr *) &remoteclient, sizeof(remoteclient)) != 0) 
    {
            printf("connection failed\n");
    }
    else
    {
            printf("Connection success\n");
    }

    uint8_t buffer[50000];

    int bytes = read(sockfd, (void *) buffer, sizeof(buffer));

    if(bytes < 1 )
    {
            perror("read");
    }

    Payload *d;

    printf("BYTES : %d\n", bytes);

    d = payload__unpack(NULL, bytes, buffer);

    if(d == NULL)
    {
            printf("NULL");
            return 0;
    }

    SERVERHELLO *shello = d->serverhello;

    printf("RECEIVED - A : %s B :%d \n", shello->ip, shello->port);

}
