#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"base64.h"

char lookuptable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *b64encode(char *message)
{
    int len = strlen(message), i, j;
    int hashlen = (len * 4)/3  + (len % 3) + 1;
    int padding = 0;
    int k = 0;

    char *hash = (char *) calloc(hashlen, sizeof(char));

    for(i = 0; i<len; i = i+3)
    {
            uint32_t value = 0;
            int count = 0;
            int itr = i;

            for(itr = i; itr < len && itr < i+3; itr++)
            {
                    value = (value << 8);
                    value = value | message[itr];
                    count++;
            }
        

            int no_bits = count * 8;
            padding = 3 - count;


            int index = -1;

            while(no_bits != 0)
            {
                    if(no_bits >= 6)
                    {    
                            int temp = no_bits - 6;
                            index = (value >> temp) & 0x3F;
                            no_bits -= 6;
                    }
                    else
                    {
                            int left = 6 - no_bits;

                            index = (value << left) & 0x3F;
                            no_bits = 0;
                    }

                    hash[k++] = lookuptable[index];
            }
    }

    for(i=0; i<padding; i++)
    {
            hash[k++] = '=';
    }

    return hash;

}

int getb64int(char a)
{
        int i;

        int n = sizeof(lookuptable)/sizeof(lookuptable[0]);

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