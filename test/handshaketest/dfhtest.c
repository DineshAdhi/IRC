#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<fcntl.h>

#include "../../utilities/common/commonutil.h"

#define G 2
#define P 57
#define URANDOM_FILE "/dev/urandom"

long i;
int itr = 0;

int GENERATE_RANDOM()
{
        int r;

        FILE *urandom = fopen(URANDOM_FILE, "rb");

        do 
        {
                if( (r = fgetc(urandom)) == EOF)
                {
                      
                }

        } while(r >= (UCHAR_MAX + 1)/ RANDOMLEN * RANDOMLEN);

        return r;

}

char RAND[63] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

uint8_t *createRandomKey()
{
        int i;

        uint8_t *key = (uint8_t *)calloc(KEYLENGTH, sizeof(uint8_t));


        for(i=0; i<KEYLENGTH; i++)
        {
                int r = GENERATE_RANDOM();
                key[i] = r % 10;
        }

        return key;
}

uint8_t *createDFHKey(uint8_t *key)
{
        int i;

        uint8_t *dfhkey = (uint8_t*) calloc(KEYLENGTH, sizeof(uint8_t));

        for(i=0; i<KEYLENGTH; i++)
        {
                dfhkey[i] = DFH(DFH_G, key[i]);
        }

        return dfhkey;
}

uint8_t *resolveDFHKey(uint8_t *secretkey, uint8_t *publickey)
{
        int i;

        uint8_t *rKey = (uint8_t *) calloc(KEYLENGTH, sizeof(uint8_t));

        for(i=0; i<KEYLENGTH; i++)
        {
                rKey[i] = DFH(publickey[i], secretkey[i]);
        }

        return rKey;
}

void printKey(uint8_t *key)
{
        int i;

        // char *hash = (char *) malloc(KEYLENGTH * 2);
        // int itr = 0;

        // for(i=0; i<KEYLENGTH * 2 && itr < KEYLENGTH; i=i+2)
        // {
        //         printf("%d ", key[itr++]);
        //         fflush(stdout);
        // }

        for(i=0; i<KEYLENGTH; i++)
        {
            printf("%d ", key[i]);
            fflush(stdout);
        }

        printf("\n");
        fflush(stdout);
}

int main()
{
    uint8_t *a = createRandomKey();
    uint8_t *b = createRandomKey();

    printf("A RANDOM : "); printKey(a);
    printf("\nB RANDOM : "); printKey(b);

    uint8_t *dfh_a = createDFHKey(a);
    uint8_t *dfh_b = createDFHKey(b);

    printf("\nA DFH : "); printKey(dfh_a);
    printf("\nB DFH : "); printKey(dfh_b);

    uint8_t *msga = resolveDFHKey(a, dfh_b);
    uint8_t *msgb = resolveDFHKey(b, dfh_a);

    printf("\nA SEC : "); printKey(msga);
    printf("\nB SEC : "); printKey(msgb);

    printf("\n\nA DFH : "); printKey(dfh_a);
    printf("\n\nB DFH : "); printKey(dfh_b);

    for(i=0; i<KEYLENGTH; i++)
    {
        if(msga[i] != msgb[i])
        {
            printf("\n\nTEST FAILED");
            return 0;
        }
    }



    printf("\n\nTEST PASSED");
}