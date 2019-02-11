#include <stdio.h>
#include <stdlib.h>
#include "amessage.pb-c.h"

#include "../../utilities/crypto/aes256.h"

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

        fprintf(stderr, "\n");
        fflush(stderr);
}

int main (int argc, const char * argv[]) 
{
  AMessage msg = AMESSAGE__INIT; 
  void *buf;                     
  unsigned len;            

  char *key = "asdfasdfasdfasdfasdfasdfasdfasdf";  
  AES_WRAPPER *w = ini_aes256_wrapper((uint8_t *)key);    
  
  if (argc != 2 && argc != 3)
  {   
    fprintf(stderr,"usage: amessage a [b]\n");
    return 1;
  }
  
  msg.a = atoi(argv[1]);
  if (argc == 3) { msg.has_b = 1; msg.b = atoi(argv[2]); }
  len = amessage__get_packed_size(&msg);
  
  buf = malloc(len);
  amessage__pack(&msg,buf);

  w->plain = buf;
  w->length = len;
  wrapper_aes256_encrypt(w);

  print(w->plain, w->length);
  
  fprintf(stderr,"Writing %d serialized bytes\n",len); 
  fflush(stderr);
  fwrite(w->hash,w->length,1,stdout); 
  
  free(buf); 
  return 0;
}
