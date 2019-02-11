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
                fprintf(stdout, "%02X ", key[itr++]);
                fflush(stdout);
        }

        printf("\n");
        fflush(stdout);
}

int main (int argc, const char * argv[]) 
{
    AMessage msg = AMESSAGE__INIT;                    

    char *key = "asdfasdfasdfasdfasdfasdfasdfasdf";  
    AES_WRAPPER *w = ini_aes256_wrapper((uint8_t *)key); 
    AES_WRAPPER *wr = ini_aes256_wrapper((uint8_t *)key); 
    
    if (argc != 2 && argc != 3)
    {   
        fprintf(stderr,"usage: amessage a [b]\n");
        return 1;
    }
    
    msg.a = atoi(argv[1]);
    if (argc == 3) { msg.has_b = 1; msg.b = atoi(argv[2]); }


    w->length = amessage__get_packed_size(&msg);
    
    w->plain = (uint8_t *) calloc(w->length, sizeof(uint8_t));
    amessage__pack(&msg, w->plain);
    wrapper_aes256_encrypt(w);


    wr->hash = w->hash;
    wr->length = w->length;

    wrapper_aes256_decrypt(wr);

    AMessage *amsg = amessage__unpack(NULL, wr->length, wr->plain);

    if(amsg == NULL)
    {
        printf("ERROR HERE");
    }
    else 
    {
        printf("SUCCESS %d", amsg->a);
        
    }

    return 0;
}
