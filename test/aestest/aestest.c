#include<stdio.h>
#include<string.h>

#include "../../utilities/crypto/aes256.h"
#include "../../utilities/crypto/base64.h"

int main()
{
    char *key = "dinasdfasfeshdineshdineshdineshdinesheqfasdfadfs";
    AES_CTX *ctx = init_aes356_ctx((uint8_t *)key);

    char *secret = "Dinesh Adhithya";
    int len = strlen(secret);
    int i;

    printf("LEN : %d \n\n", len);

    char *hash = (char *) aes256_encrypt(ctx, (uint8_t *)secret, len);

    char *base64 = b64encode(hash, strlen(hash));


    for(i=0; i<len; i++)
    {
        printf("%c", base64[i]);
    }


    printf("\n");

    char *plain = (char *) aes256_decrypt(ctx, (uint8_t *)hash, len);


    for(i=0; i<len; i++)
    {
        printf("%c", (char)plain[i]);
    }

    

}