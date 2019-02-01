#include<stdio.h>
#include<string.h>

#include "../../utilities/crypto/aes256.h"
#include "../../utilities/crypto/base64.h"

int main()
{
    int i;

    char *key = "zohowms_mps_secretkey";
    AES_CTX *ctx = init_aes356_ctx((uint8_t *)key);

    char *secret = "GoogleAppleMicrosoftIRCServerIRCClient";
    int len = strlen(secret);

    uint8_t *base64secret = b64encode((uint8_t*)secret, len);

    len = strlen((char *)base64secret);


    for(i=0; i<len; i++)
    {
        printf("%0c", base64secret[i]);
    }

    printf("\n");

    uint8_t *hash = aes256_encrypt(ctx, (uint8_t *)base64secret, len);

    for(i=0; i<len; i++)
    {
        printf("%0c", hash[i]);
    }


    printf("\n");

    uint8_t *plain = aes256_decrypt(ctx, (uint8_t *)hash, len);

    uint8_t *base64plain = b64decode(plain, strlen((char*)plain));

    for(i=0; i<strlen(plain); i++)
    {
         printf("%c", plain[i]);
    }

    printf("\n");

    for(i=0; i<strlen(base64plain); i++)
    {
         printf("%c", base64plain[i]);
    }

    printf("\n\n\n");

    if(strcmp(secret, base64plain) == 0)
    {
        printf("TEST PASSED");
    }
    else 
    {
        printf("TEST FAILED");
    }


    return 0;
}