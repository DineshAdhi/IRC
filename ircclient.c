#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<unistd.h>

#include"utilities/client/clientutil.h"
#include"utilities/client/clienthandler.h"
#include"utilities/logger/log.h"
#include"utilities/common/commonutil.h"

int main()
{
      initiateIRCClient();
      struct sockaddr_in *remoteaddr = getremoteserveraddr();
      clientfd = createSocket();
      int maxfd = clientfd + 1;
      fd_set read_fds, write_fds, except_fds;

      connect_to_server(clientfd, *remoteaddr);

      while(TRUE)
      {
            maxfd = preparefds_client(clientfd, &read_fds, &write_fds, &except_fds);

            int activity = select(maxfd, &read_fds, &write_fds, &except_fds, NULL);

            switch(activity)
            {
                  case -1:
                  case 0:
                     perror("select()");
                     exit(1);
                     break;
                  
                  default:

                     if(FD_ISSET(STDIN_FILENO, &read_fds))
                     {
                           handle_stdin_data();
                     }

                     if(FD_ISSET(clientfd, &read_fds))
                     {
                           if(read_from_server() < 0)
                           {
                                 exit(1);
                           }
                     }
            }
      }
}