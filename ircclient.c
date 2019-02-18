#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<unistd.h>

#include"include/clientutil.h"
#include"include/clienthandler.h"
#include"include/log.h"
#include"include/commonutil.h"

int main()
{
      initiateIRCClient();
      fd_set read_fds, write_fds, except_fds;

      initiate_connect_to_server(); 
      int maxfd = serverconn->fd + 1;

      while(TRUE)
      {
            preparefds_client(&read_fds, &write_fds, &except_fds);

            int activity = select(maxfd, &read_fds, &write_fds, &except_fds, NULL);

            switch(activity)
            {
                  case -1:
                  case 0:
                  {
                     perror("select()");
                     exit(1);
                     break;
                  }
                  
                  default:

                     if(FD_ISSET(serverconn->fd, &read_fds))
                     {
                              handle_io_client();
                     }

                     if(FD_ISSET(serverconn->fd, &write_fds))
                     {
                              handle_io_client();
                     }

                     if(FD_ISSET(serverconn->fd, &except_fds))
                     {
                           log_debug("[EXCEPTION IN SELECT]");
                           deregisterServer();
                     }
            }
      }
}
