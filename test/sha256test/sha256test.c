#include<stdio.h>

#include"../../utilities/crypto/sha256.h"

int main()
{
    char *name = "Dinesh";
    char hash[65] = {};

    sha256(hash, name);

    printf("%s", hash);
}

