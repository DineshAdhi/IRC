#include<stdio.h>
#include<string.h>
#include"../../utilities/crypto/base64.h"

int main()
{
    char *message = "asdflakjsdflkadflkfasdfasgwerwerwasdfadfasdfdfasdfasdfasfaafasdsdfasdsdferaferfawdfsadfasdsjasdfsddfkladfsdfshdfasdfsdfjfasdfaskasdhfsdfasdfasdkalsdjfalksdhfalsdhfaljasdfasdfasdfasdfasdfsadfasdfasdfasdfsdfsdfksdhflaksdhflkwdhflkdhflwdhflkwdhflkdhfklshfls";
    size_t size = strlen(message);

    char *hash = b64encode(message, size);

    char *result = b64decode(hash);

    if(strcmp(message, result) == 0)
    {
        printf("TEST PASSED");
    }
    else 
    {
        printf("TEST FAILED");
    }
}