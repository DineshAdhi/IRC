#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"base64.h"

uint8_t lookuptable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

uint8_t *b64encode(uint8_t *data, size_t len)
{
    int i = 0, j = 0;
    size_t size = 0;
    uint8_t temp[3];
    uint8_t buf[4];
    uint8_t *hash = (uint8_t*) malloc(1);

    while(len--)
    {
            temp[i++] = *(data++);

            if(i == 3)
            {
                buf[0] = (temp[0] & 0xFC) >> 2;
                buf[1] = ((temp[0] & 0x03) << 4) + ((temp[1] & 0xF0) >> 4);
                buf[2] = ((temp[1] & 0x0F) << 2) + ((temp[2] & 0xC0) >> 6);
                buf[3] = temp[2] & 0x3F;
            
                hash = (uint8_t *) realloc(hash, size + 4);

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

            buf[0] = (temp[0] & 0xFC) >> 2;
            buf[1] = ((temp[0] & 0x03) << 4) + ((temp[1] & 0xF0) >> 4);
            buf[2] = ((temp[1] & 0x0F) << 2) + ((temp[2] & 0xC0) >> 6);
            buf[3] = temp[2] & 0x3F;

            for(j=0; j<i+1; j++)
            {
                hash = (uint8_t *) realloc(hash, size + 1);
                hash[size++] = lookuptable[buf[j]];
            }

            while((i++ < 3)) 
            {
                    hash = (uint8_t *) realloc(hash, size + 1);
                    hash[size++] = '=';
            }
    }


    hash = (uint8_t *) realloc(hash, size + 1);
    hash[size] = '\0'; 

    return hash;
}


int getb64int(uint8_t a)
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

uint8_t *b64decode(uint8_t *hash, size_t len)
{
        int i = 0, j = 0;
        uint8_t *plain = (uint8_t *) calloc(1, sizeof(uint8_t));
        size_t size = 0;
        uint8_t temp[4];
        uint8_t buf[3];

        while(len--)
        {
                temp[i++] = *(hash++);

                if(i == 4)
                {
                        for(i=0; i<4; i++)
                        {
                            temp[i] = getb64int(temp[i]);
                         }

                        buf[0] = (temp[0] << 2) + ((temp[1] & 0x30) >> 4);
                        buf[1] = ((temp[1] & 0xf) << 4) + ((temp[2] & 0x3c) >> 2);
                        buf[2] = ((temp[2] & 0x3) << 6) + temp[3];

                        plain = (uint8_t *) realloc(plain, size+3);

                        for(i=0; i<3; i++)
                        {
                            plain[size++] = buf[i];
                        }

                        i = 0;
                }
        }

        if(i > 0)
        {
                for(j=i; j<4; j++)
                {
                    temp[j] = '\0';
                }

                for(j=0; j<4; j++)
                {
                    temp[j] = getb64int(temp[j]);
                }

                buf[0] = (temp[0] << 2) + ((temp[1] & 0x30) >> 4);
                buf[1] = ((temp[1] & 0xf) << 4) + ((temp[2] & 0x3c) >> 2);
                buf[2] = ((temp[2] & 0x3) << 6) + temp[3];

                plain = (uint8_t *) realloc(plain, size + (i-1));

                for(j=0; j<i-1; j++)
                {
                    plain[size++] = buf[j];
                }
        }

        plain = (uint8_t *) realloc(plain, size+1);
        plain[size] = '\0';

        return plain;
}
