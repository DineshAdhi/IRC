#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>

#include "log.h"
#include "models.h"

int handle_incoming_connection(int serverfd, client_model *remoteclient);
