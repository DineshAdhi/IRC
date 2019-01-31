#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include"aes256.h"

void getRoundkey(uint8_t *Rkey, uint8_t *key)
{
      int i,j;
      uint8_t tempa[4];

      for(i=0; i<Nk; i++)
      {
            Rkey[(i * 4) + 0] = key[(i * 4) + 0];
            Rkey[(i * 4) + 1] = key[(i * 4) + 1];
            Rkey[(i * 4) + 2] = key[(i * 4) + 2];
            Rkey[(i * 4) + 3] = key[(i * 4) + 3];
      }

      for(i=Nk; i< Nb * (Nr + 1); i++)
      {
            int k = (i - 1) * 4;
            tempa[0] = Rkey[k + 0];
            tempa[1] = Rkey[k + 1];
            tempa[2] = Rkey[k + 2];
            tempa[3] = Rkey[k + 3];

            if(i % Nk == 0)
            {
                uint8_t t = tempa[0];
                tempa[0] = tempa[1];
                tempa[1] = tempa[2];
                tempa[2] = tempa[3];
                tempa[3] = t;

                tempa[0] = SBoxValue(tempa[0]);
                tempa[1] = SBoxValue(tempa[1]);
                tempa[2] = SBoxValue(tempa[2]);
                tempa[3] = SBoxValue(tempa[3]);

                tempa[0] = tempa[0] ^ Rcon[i/Nk];
            }

            if(i % Nk == 4)
            {
                tempa[0] = SBoxValue(tempa[0]);
                tempa[1] = SBoxValue(tempa[1]);
                tempa[2] = SBoxValue(tempa[2]);
                tempa[3] = SBoxValue(tempa[3]);
            }

            j = i * 4; k = (i - Nk) * 4;
            
            Rkey[j + 0] = Rkey[k + 0] ^ tempa[0];
            Rkey[j + 1] = Rkey[k + 1] ^ tempa[1];
            Rkey[j + 2] = Rkey[k + 2] ^ tempa[2];
            Rkey[j + 3] = Rkey[k + 3] ^ tempa[3];
      }
}