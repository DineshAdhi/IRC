#include<stdio.h>
#include<string.h>

#include "../../include/aes256.h"
#include "../../include/base64.h"

void print(uint8_t *key, int len)
{
        int i;

        char *hash = (char *) calloc(len, 2);
        int itr = 0;

        for(i=0; i<len * 2; i=i+2)
        {
                fprintf(stderr, "%02X ", key[itr++]);
                fflush(stdout);
        }

        printf("\n");
        fflush(stderr);
}

int main()
{
    int i;

    char *key = "2b7e151628aed2a6abf7158809cf4f3c";

    char *secret = "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710";
    int len = strlen(secret);

    AES_WRAPPER *w = init_aes256_wrapper((uint8_t *)key);
    w->plain = (uint8_t *) secret;
    w->length = len;

    wrapper_aes256_encrypt(w);

    for(i=0; i<w->length; i++)
    {
        printf("%c", w->hash[i]);
    }

//    printf("\n BASE 64 : %s", w->base64);
    
  //  printf("\n HEX : %s", w->hex);
    /*w->plain = NULL;
    wrapper_aes256_decrypt(w);

    for(i=0; i<len; i++)
    {
        printf("%d ", w->hash[i]);
    }

    printf("\n Decrypt : %s", w->plain);*/
}
