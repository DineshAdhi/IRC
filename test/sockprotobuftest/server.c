#include<stdio.h>
#include<stdlib.h>
#include "../../utilities/netutil.h"
#include "data.pb-c.h"

int main(int argc, char *argv[])
{
        Data d = DATA__INIT;
        d.a = 546;
        d.b = 932;
        d.str = "dinesh";

        int packedlen = data__get_packed_size(&d);
        void *buffer;
        buffer = malloc(packedlen);

        data__pack(&d, buffer);

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
       write(connfd, buffer, len);
}
}
