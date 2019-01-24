#include<stdio.h>
#include<string.h>
#include"../../utilities/crypto/base64.h"

int main()
{
    char *message = "ads23423ewrfwsadfcasdfasdfasgdsfgsdfgsdfhdtr4ewrdfsszdcsxzcasdffasdf3ewrdfsacxafasdfasdfsdgdsfgdfsdfasdfasereasdfasasdfasdfasdfasdfasdfasdxfsdsh";

    char *hash = b64encode(message);

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