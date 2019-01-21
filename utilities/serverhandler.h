#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>

#include "log.h"

int handle_incoming_connection(int serverfd);
void handle_data_from_stdin();
