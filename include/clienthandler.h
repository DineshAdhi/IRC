#include<stdlib.h>
#include<stdio.h>  
#include<arpa/inet.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<unistd.h>
#include<signal.h>

void handle_stdin_data();
void handle_io_client();