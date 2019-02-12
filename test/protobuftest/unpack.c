#include <stdio.h>
#include <stdlib.h>
#include "amessage.pb-c.h"
#define MAX_MSG_SIZE 1024

#include "../../utilities/crypto/aes256.h"

static size_t
read_buffer (unsigned max_length, uint8_t *out)
{
  size_t cur_len = 0;
  size_t nread;
  while ((nread=fread(out + cur_len, 1, max_length - cur_len, stdin)) != 0)
  {
    cur_len += nread;
    if (cur_len == max_length)
    {
      fprintf(stderr, "max message length exceeded\n");
      exit(1);
    }
  }
  return cur_len;
}

void print(uint8_t *key, int len)
{
        int i;

        char *hash = (char *) calloc(len, 2);
        int itr = 0;

        for(i=0; i<len * 2; i=i+2)
        {
                fprintf(stderr, "%02X ", key[itr++]);
                fflush(stderr);
        }

        printf("\n");
        fflush(stderr);
}


int main (int argc, const char * argv[]) 
{
  AMessage *msg;

  char *key = "asdfasdfasdfasdfasdfasdfasdfasdf";  
  AES_WRAPPER *w = ini_aes256_wrapper((uint8_t *)key);    

  w->hash = (uint8_t *) calloc(1000, sizeof(uint8_t));
  w->length = read_buffer (MAX_MSG_SIZE, w->hash);

  print(w->hash, w->length);

  wrapper_aes256_decrypt(w);


  msg = amessage__unpack(NULL, w->length, w->plain);	
  if (msg == NULL)
  {
    fprintf(stderr, "error unpacking incoming message\n");
    exit(1);
  }


  printf("Received: a=%d",msg->a);  
  if (msg->has_b)                   
    printf("  b=%d",msg->b);
  printf("\n");

  amessage__free_unpacked(msg, NULL);
  return 0;
}
