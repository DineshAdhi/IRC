#include<stdio.h>
#include<string.h>
#include"../../utilities/crypto/base64.h"

int main()
{
    char *message = "sdfadfasfasdfasdfasdfasdfasdfasdfasdfasdfasdfasfd";
    size_t size = strlen(message);

    char *hash = (char *) b64encode((uint8_t*)message, size);

    printf("%s\n", hash);

    char *result = (char *) b64decode((uint8_t*)hash, strlen(hash));

    printf("%s\n\n%s\n\n", message, result);

    if(strcmp(message, result) == 0)
    {
        printf("TEST PASSED");
    }
    else 
    {
        printf("TEST FAILED");
    }
}
