#include<stdio.h>
#include<string.h>

#include "../../utilities/crypto/aes256.h"
#include "../../utilities/crypto/base64.h"

int main()
{
    int i;

    char *key = "qwertyuiopasdfghjklzxcvbnmqwerty";

    char *secret = "dineshdineshdine";
    int len = strlen(secret);     

    AES_WRAPPER *w = ini_aes256_wrapper((uint8_t *)key);
    w->plain = (uint8_t *) secret;
    w->length = len;

    wrapper_aes256_encrypt(w);

    for(i=0; i<len; i++)
    {
        printf("%c", w->hash[i]);
    }

    printf("\n HEX : %s", w->hex);
    printf("\n BASE 64 : %s", w->base64);
}
