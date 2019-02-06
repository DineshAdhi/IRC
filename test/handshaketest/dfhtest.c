#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>

#include "../../utilities/common/commonutil.h"

#define G 2
#define P 57


long i;

int main()
{
    uint8_t *a = createRandomKey();
    uint8_t *b = createRandomKey();

    uint8_t *dfh_a = createDFHKey(a);
    uint8_t *dfh_b = createDFHKey(b);

    uint8_t *msga = resolveDFHKey(a, dfh_b);
    uint8_t *msgb = resolveDFHKey(b, dfh_a);

    for(i=0; i<KEYLENGTH; i++)
    {
        if(msga[i] != msgb[i])
        {
            printf("TEST FAILED");
            return 0;
        }
    }

    printf("TEST PASSED");
}