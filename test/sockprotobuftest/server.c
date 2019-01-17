#include<stdio.h>
#include<stdlib.h>
#include "../../utilities/netutil.h"
#include "data.pb-c.h"

int main(int argc, char *argv[])
{
        Data d = DATA__INIT;
        d.a = atoi(argv[1]);
        d.b = atoi(argv[2]);
        d.str = argv[3];

        int packedlen = data__get_packed_size(&d);
        void *buffer;
        buffer = malloc(packedlen);

        data__pack(&d, buffer);

        Data *temp;

        temp = data__unpack(NULL, packedlen, buffer);

        printf("SENDING - A : %d B : %d Str : %s LEN : %d\n\n", temp->a, temp->b, temp->str, packedlen);
        fflush(stdout);
       struct sockaddr_in *addr = getserversockAddr();
       struct sockaddr_in client;
       socklen_t len;

       int serverfd = createSocket();
       bindsocket(serverfd, *addr);
       listenforconnections(serverfd);
        
       len = sizeof(client);
while(1)
{
       int connfd = accept(serverfd, (struct sockaddr *) &client, &len);
       write(connfd, buffer, packedlen);
}
}
