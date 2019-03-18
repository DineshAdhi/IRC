#include<stdlib.h>
#include<stdio.h>  
#include<string.h>
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<signal.h>
#include<netdb.h>

#include"../../include/commonutil.h"
#include"../../include/log.h"
#include"../../include/clientutil.h"
#include"../../include/sha256.h"

void terminateClient()
{
        log_info("[IRCCLIENT][SIGINT/SIGTSTP RECEIVED][HALTING SERVER]");
        close(serverconn->fd);
        fclose(clientlog);
        exit(1);
}

void loadUserConfigFile()
{
      configfile = fopen(USERCONFIG_FILE_PATH, "r");

      if(configfile == NULL)
      {
            log_info("[USER CONFIG IS EMPTY]");
            isAuthRequired = REQUIRED;
            return;
      }

      fseek(configfile, 0, SEEK_END);
      size_t file_size = ftell(configfile);

      if(file_size == 0)
      {
            log_info("[USER CONFIG IS EMPTY]");
            isAuthRequired = REQUIRED;
            return;
      }

      uint8_t buffer[file_size]; 
      char ch;
      int itr = 0;
      fseek(configfile, 0, SEEK_SET);

      while( (ch = fgetc(configfile)) != EOF){
            buffer[itr++] = (uint8_t) ch;
      }

      userconfig = user_config__unpack(NULL, file_size, buffer);

      if(userconfig == NULL)
      {
            log_info("[USER CONFIG FILE CORRUPTED]");
            isAuthRequired = REQUIRED;
      }
      else
      {
            log_info("CONFIG FILE LOADED SUCCESSFULLY FOR USER : %s", userconfig->id);
            isAuthRequired = NOT_REQUIRED;
      }

      fclose(configfile);
}



void saveConfigFile()
{
      configfile = fopen(USERCONFIG_FILE_PATH, "w");

      size_t len = user_config__get_packed_size(userconfig);
      uint8_t buffer[len];

      user_config__pack(userconfig, buffer);

      fwrite(buffer, len, 1, configfile);
      fclose(configfile);
}

void initiateIRCClient()
{
        struct sigaction action;
        action.sa_handler = terminateClient;

        if(sigaction(SIGINT, &action, 0) < 0)
        {
                perror("sigaction");
                log_warn("[SIGACTION EXCEPTION][SIGNT]");
        }

        if(sigaction(SIGTSTP, &action, 0) < 0)
        {
                perror("sigaction");
                log_warn("[SIGACTION EXCEPTION][SIGTSTP]");
        }

        serverconn = (Connection *) calloc(1, sizeof(Connection));
        serverconn->fd = NO_FD;
        serverconn->port = -1;
        serverconn->ip = NULL;
        serverconn->payload = NULL;
        serverconn->writable = NOT_WRITABLE;
        serverconn->stage = UNKNOWN_STAGE;
        serverconn->registered = NOT_REGISTERED;
        serverconn->authdone = UNAUTHENTICATED;
        serverconn->sid = "111111";

        userconfig = (UserConfig *) calloc(1, sizeof(UserConfig));

        initializeCommonUtils();

        clientlog = fopen(CLIENT_LOGFILE_PATH, "w+");

        #if defined(CLIENT_DEBUG) && (CLIENT_DEBUG == 1)
                  log_set_quiet(0);
        #else
                  log_set_quiet(1);
        #endif
      
        loadUserConfigFile();

        log_set_fp(clientlog);
}

struct sockaddr_in getremoteserveraddr()
{
         struct hostent *host = (struct hostent *) calloc(0, sizeof(struct hostent));
         host = gethostbyname(REMOTE_SERVER_DOMAIN);

         struct sockaddr_in *addr = (struct sockaddr_in *) calloc(1, sizeof(struct sockaddr_in));
         addr->sin_family = SOCKET_FAMILY;
         addr->sin_port = htons(PORT);
         //addr->sin_addr.s_addr = inet_addr(REMOTE_SERVER_IP);
         addr->sin_addr.s_addr = *((int *)host->h_addr_list[0]);

         return *addr;
}


int _connect_to_server(int reconnect)
{
      serverconn->fd = createSocket();

      struct sockaddr_in addr = getremoteserveraddr();

      int result = connect(serverconn->fd, (struct sockaddr *)&addr, sizeof(addr));
      
      if(result < 0)
      {
            if(reconnect == !RECONNECT)
            {
                  log_fatal("[IRCCLIENT][FATAL][CONNECTION REFUSED]");
                  perror("connect()");
                  exit(1);
            }
            else
            {
                  return FAILURE;
            }
      }

      char ip[16] = {};
      int port;

      extract_addr_info(addr, ip, &port);
      
      serverconn->ip = ip;
      serverconn->port = port;
      serverconn->registered = REGISTERED;
      serverconn->payload = (IRCPayload *) calloc(1, sizeof(IRCPayload));
      serverconn->stage = MESSAGE_TYPE__clienthello;
      serverconn->secure = NOT_SECURE;
      serverconn->writable = WRITABLE;
      serverconn->handshakedone = HANDSHAKE_NOT_DONE;
      serverconn->randomkey = createRandomKey();

      log_info("[IRCCLIENT][CONNECTED TO SERVER][REMOTE SERVER IP - %s][PORT - %d]", serverconn->ip, serverconn->port);

      return SUCCESS;
}

void preparefds_client(fd_set *read_fds, fd_set *write_fds, fd_set *except_fds)
{
      FD_ZERO(read_fds);
      FD_ZERO(write_fds);
      FD_ZERO(except_fds);

      FD_SET(serverconn->fd, read_fds);
      
      if(serverconn->writable == WRITABLE)
      {
            FD_SET(serverconn->fd, write_fds);
      }

       FD_SET(STDIN_FILENO, read_fds);
}

void deregisterServer()
{
      log_info("[DEREGISTERING SERVER]");
      close(serverconn->fd);
      exit(1);
}