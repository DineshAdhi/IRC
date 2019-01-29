#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"base64.h"

char lookuptable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *b64encode(char *data, size_t len)
{
    int i = 0, j = 0;
    size_t size = 0;
    unsigned char temp[3];
    unsigned char buf[4];
    char *hash = (char *) malloc(1);

    while(len--)
    {
            temp[i++] = *(data++);

            if(i == 3)
            {
                buf[0] = (temp[0] & 0xfc) >> 2;
                buf[1] = ((temp[0] & 0x03) << 4) + ((temp[1] & 0xf0) >> 4);
                buf[2] = ((temp[1] & 0x0f) << 2) + ((temp[2] & 0xc0) >> 6);
                buf[3] = temp[2] & 0x3f;
            
                hash = (char *) realloc(hash, size + 4);

                for(i=0; i<4; i++)
                {
                        hash[size++] = lookuptable[buf[i]];
                }

                i = 0;
            }
    }

    if(i > 0)
    {
            for(j=i; j<3; j++)
            {
                temp[j] = '\0';
            }

            buf[0] = (temp[0] & 0xfc) >> 2;
            buf[1] = ((temp[0] & 0x03) << 4) + ((temp[1] & 0xf0) >> 4);
            buf[2] = ((temp[1] & 0x0f) << 2) + ((temp[2] & 0xc0) >> 6);
            buf[3] = temp[2] & 0x3f;

            for(j=0; j<i+1; j++)
            {
                hash = (char *) realloc(hash, size + 1);
                hash[size++] = lookuptable[buf[j]];
            }

            while((i++ < 3)) 
            {
                    hash = (char *) realloc(hash, size + 1);
                    hash[size++] = '=';
            }
    }


    hash = (char *) realloc(hash, size + 1);
    hash[size] = '\0'; 

    return hash;
}


int getb64int(char a)
{
        int i;

        int n = 64;

        for(i=0; i<n; i++)
        {
                if(lookuptable[i] == a)
                    return i;
        }

        return 0;
}

char* b64decode(char *hash)
{
        int len = strlen(hash);
        int i = 0;

        int plain_len = (len * 3)/4;

        char *plain = (char *) calloc(plain_len, sizeof(char));

        int k = 0;

        int choice = 0, index;

        for(i=0; i<len-1; i++)
        {
                int a = getb64int(hash[i]), b = getb64int(hash[i+1]);  
 
                switch(choice)
                {
                        case 0:
                            index = (a << 2) | ( (b>>4) & 0x03);
                            plain[k++] = (char) index;
                            choice = 1;
                            break;

                        case 1:
                            index = ( a << 4) | ( (b >> 2) & 0x0F);
                            plain[k++] = (char) index;
                            choice = 2;
                            break;

                        case 2:
                            index = ( a << 6) | ( b & 0x3F);
                            plain[k++] = (char) index;
                            i = i + 1;
                            choice = 0;
                            break;
                }
        }

        plain[k] = '\0';

        return plain;
}