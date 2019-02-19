#include<stdio.h>

#include"../../include/sha256.h"



int main()
{
    char *name = "dinesh";
    char hash[65] = {};

    sha256(hash, name);

    printf("%s\n", hash);

    char *n = "dinesh";
    char h[65] = {};

    sha256(h, n);

    printf("%s", h);
}

