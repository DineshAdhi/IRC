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

                     if(FD_ISSET(STDIN_FILENO, &read_fds))
                     {
                           handle_stdin_data();
                     }

                     if(FD_ISSET(serverconn->fd, &read_fds))
                     {
                              if( handle_io_client() == FAILURE)
                              {
                                    exit(1);
                              }
                     }

                     if(FD_ISSET(serverconn->fd, &write_fds))
                     {
                              if( handle_io_client() == FAILURE)
                              {
                                    exit(1);
                              }
                     }
            }
      }
}
