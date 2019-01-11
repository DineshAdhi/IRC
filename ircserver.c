#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include"utilities/netutil.h"

int main()
{
        struct sockaddr_in *addr = getserversockAddr();
        int sockfd = createSocket();
        
        bindsocket(sockfd, *addr);
        
        printf("[IRCSERVER][LISTEING TO - %s][PORT - %d]", SERVER_IP, PORT); 
        
}
