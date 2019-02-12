#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#include"../../include/aes256.h"
#include"../../include/base64.h"
#include"../../include/commonutil.h"

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
                uint8_t u8tmp = tempa[0];
                tempa[0] = tempa[1];
                tempa[1] = tempa[2];
                tempa[2] = tempa[3];
                tempa[3] = u8tmp;

                tempa[0] = getSBoxValue(tempa[0]);
                tempa[1] = getSBoxValue(tempa[1]);
                tempa[2] = getSBoxValue(tempa[2]);
                tempa[3] = getSBoxValue(tempa[3]);

                tempa[0] = tempa[0] ^ Rcon[i/Nk];
            }

            if(i % Nk == 4)
            {
                tempa[0] = getSBoxValue(tempa[0]);
                tempa[1] = getSBoxValue(tempa[1]);
                tempa[2] = getSBoxValue(tempa[2]);
                tempa[3] = getSBoxValue(tempa[3]);
            }

            j = i * 4; k = (i - Nk) * 4;
            
            Rkey[j + 0] = Rkey[k + 0] ^ tempa[0];
            Rkey[j + 1] = Rkey[k + 1] ^ tempa[1];
            Rkey[j + 2] = Rkey[k + 2] ^ tempa[2];
            Rkey[j + 3] = Rkey[k + 3] ^ tempa[3];
      }
}

static void add_round_key(state_t *s, uint8_t *rkey, int round)
{
      int i,j;

      for(i=0; i<4; i++)
      {
            for(j=0; j<4; j++)
            {
                  (*s)[i][j] ^= rkey[(round * Nb * 4) + (i * Nb) + j];
            }
      }
}

static void subbytes(state_t *s)
{
      int i,j;

      for(i=0; i<4; i++)
      {
            for(j=0; j<4; j++)
            {
                  (*s)[i][j] = getSBoxValue((*s)[i][j]);
            }
      }
}

static void invsubbytes(state_t* state)
{
      uint8_t i, j;

      for (i = 0; i < 4; ++i)
      {
            for (j = 0; j < 4; ++j)
            {
                  (*state)[j][i] = getSBoxInvert((*state)[j][i]);
            }
      }
}

static void shiftrows(state_t *s)
{
      uint8_t temp;

      temp = (*s)[0][1];
      (*s)[0][1] = (*s)[1][1];
      (*s)[1][1] = (*s)[2][1];
      (*s)[2][1] = (*s)[3][1];
      (*s)[3][1] = temp;

      temp = (*s)[0][2];
      (*s)[0][2] = (*s)[2][2];
      (*s)[2][2] = temp;

      temp = (*s)[1][2];
      (*s)[1][2] = (*s)[3][2];
      (*s)[3][2] = temp;

      temp = (*s)[0][3];
      (*s)[0][3] = (*s)[3][3];
      (*s)[3][3] = (*s)[2][3];
      (*s)[2][3] = (*s)[1][3];
      (*s)[1][3] = temp;
}

static void invshiftrows(state_t* s)
{
      uint8_t temp;

      temp = (*s)[3][1];
      (*s)[3][1] = (*s)[2][1];
      (*s)[2][1] = (*s)[1][1];
      (*s)[1][1] = (*s)[0][1];
      (*s)[0][1] = temp;

      temp = (*s)[0][2];
      (*s)[0][2] = (*s)[2][2];
      (*s)[2][2] = temp;

      temp = (*s)[1][2];
      (*s)[1][2] = (*s)[3][2];
      (*s)[3][2] = temp;

      temp = (*s)[0][3];
      (*s)[0][3] = (*s)[1][3];
      (*s)[1][3] = (*s)[2][3];
      (*s)[2][3] = (*s)[3][3];
      (*s)[3][3] = temp;
}


static uint8_t xtime(uint8_t x)
{
      return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
}

static uint8_t Multiply(uint8_t x, uint8_t y)
{
  return (((y & 1) * x) ^
       ((y>>1 & 1) * xtime(x)) ^
       ((y>>2 & 1) * xtime(xtime(x))) ^
       ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^
       ((y>>4 & 1) * xtime(xtime(xtime(xtime(x)))))); 
}

static void mixcolumns(state_t *s)
{
      uint8_t i;
      uint8_t Tmp, Tm, t;
      for (i = 0; i < 4; ++i)
      {  
                  t   = (*s)[i][0];
                  Tmp = (*s)[i][0] ^ (*s)[i][1] ^ (*s)[i][2] ^ (*s)[i][3] ;
                  Tm  = (*s)[i][0] ^ (*s)[i][1] ; Tm = xtime(Tm);  (*s)[i][0] ^= Tm ^ Tmp ;
                  Tm  = (*s)[i][1] ^ (*s)[i][2] ; Tm = xtime(Tm);  (*s)[i][1] ^= Tm ^ Tmp ;
                  Tm  = (*s)[i][2] ^ (*s)[i][3] ; Tm = xtime(Tm);  (*s)[i][2] ^= Tm ^ Tmp ;
                  Tm  = (*s)[i][3] ^ t ;              Tm = xtime(Tm);  (*s)[i][3] ^= Tm ^ Tmp ;
      }
}

static void invmixcolumns(state_t* s)
{
      int i;
      uint8_t a, b, c, d;

      for (i = 0; i < 4; ++i)
      { 
            a = (*s)[i][0];
            b = (*s)[i][1];
            c = (*s)[i][2];
            d = (*s)[i][3];

            (*s)[i][0] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
            (*s)[i][1] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
            (*s)[i][2] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
            (*s)[i][3] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
      }
}

void cipher(state_t *s, uint8_t *rkey)
{
      int i,j;
      int round = 0;
      add_round_key(s, rkey, 0);

      for(i=1; i<Nr; i++)
      {
            subbytes(s);
            shiftrows(s);
            mixcolumns(s);
            add_round_key(s, rkey, i);
      }

      subbytes(s);
      shiftrows(s);
      add_round_key(s, rkey, Nr);
}

static void decipher(state_t* s,uint8_t* RoundKey)
{
      uint8_t round = 0;

      add_round_key(s, RoundKey, Nr); 

      for (round = (Nr - 1); round > 0; --round)
      {
            invshiftrows(s);
            invsubbytes(s);
            add_round_key(s, RoundKey, round);
            invmixcolumns(s);
      }
      
      invshiftrows(s);
      invsubbytes(s);
      add_round_key(s, RoundKey, 0);
}

AES_CTX *init_aes256_ctx(uint8_t *key)
{
      AES_CTX *ctx = (AES_CTX *) calloc(1, sizeof(AES_CTX));
      getRoundkey(ctx->rkey, key);

      return ctx;
}

state_t *copy_to_state(uint8_t *plain, int mark, int len)
{
      int i, j;
      state_t *s = (state_t *)calloc(1, sizeof(state_t)); 

      for(i=0; i<4; i++)
      {
            for(j=0; j<4; j++)
            {
                  (*s)[i][j] = plain[mark++];
            }
      }

      return s;
}

void copy_to_buffer(state_t *s, uint8_t *enc, int mark, int len)
{
      int i,j;

      for(i=0; i<4; i++)
      {
            for(j=0; j<4; j++)
            {
                  enc[mark++] = (*s)[i][j];
            }
      }
}

uint8_t* aes256_encrypt(AES_CTX *ctx, uint8_t *plain, size_t length)
{
     int  i = 0, j, itr = 0;

     state_t *s = (state_t *) calloc(1, sizeof(state_t));
     uint8_t *hash = (uint8_t *) calloc(length, sizeof(char));

     for(i=0; i<length; i=i+AESBLOCKLEN)
     {
            s = copy_to_state(plain, i, length);
            cipher(s, ctx->rkey);
            copy_to_buffer(s, hash, i, length);
     }

     return hash;
}

uint8_t* aes256_decrypt(AES_CTX *ctx, uint8_t *hash, size_t length)
{
     int  i = 0, j, itr = 0;

     state_t *s = (state_t *) calloc(1, sizeof(state_t));
     uint8_t *plain = (uint8_t *) calloc(length, sizeof(uint8_t));

     for(i=0; i<length; i=i+AESBLOCKLEN)
     {
            s = copy_to_state(hash, i, length);
            decipher(s, ctx->rkey);
            copy_to_buffer(s, plain, i, length);
     }

     return plain;
}

AES_WRAPPER* init_aes256_wrapper(uint8_t *key)
{      
      AES_CTX *ctx = init_aes256_ctx(key);

      AES_WRAPPER *w = (AES_WRAPPER *) calloc(1, sizeof(AES_WRAPPER));
      w->ctx = ctx;
      w->plain = NULL;
      w->hash = NULL;
      w->length = -1;

      return w;
}

int wrapper_aes256_encrypt(AES_WRAPPER *w)
{
      int  i = 0, j, itr = 0;
      int rem =  16 - (w->length % 16);

      w->length += rem;

      state_t *s = (state_t *) calloc(1, sizeof(state_t));
      w->hash = (uint8_t *) calloc(w->length, sizeof(char));

      for(i=0; i<w->length; i=i+AESBLOCKLEN)
      {
            s = copy_to_state(w->plain, i, w->length);
            cipher(s, w->ctx->rkey);
            copy_to_buffer(s, w->hash, i, w->length);
      }

      #if defined(DO_ENCODING_AFTER_ENCRYPTION) && (DO_ENCODING_AFTER_ENCRYPTION == 1)

            w->base64 = (char *) b64encode(w->hash, w->length);

            w->hex = (char *) calloc(w->length * 2, sizeof(char));
            
            for(i=0; i<w->length * 2; i = i + 2)
            {
                  sprintf(w->hex + i, "%02X", w->hash[itr++]);
            }
            
      #endif

      #if defined(DELETE_PLAIN_AFETER_ENCRYPTION) && (DELETE_PLAIN_AFETER_ENCRYPTION == 1)
            w->plain = NULL;
      #endif

      return w->length;
}

int wrapper_aes256_decrypt(AES_WRAPPER *w)
{
     int  i = 0, j, itr = 0;

     state_t *s = (state_t *) calloc(1, sizeof(state_t));
     w->plain = (uint8_t *) calloc(w->length, sizeof(uint8_t));

     for(i=0; i<w->length; i=i+AESBLOCKLEN)
     {
            s = copy_to_state(w->hash, i, w->length);
            decipher(s, w->ctx->rkey);
            copy_to_buffer(s, w->plain, i, w->length);
     }

     return w->length;
}

void conn_wrapper_aes256_encrypt(Connection *c)
{
      c->aeswrapper->plain = c->buffer;
      c->aeswrapper->length = c->len;

      c->len = wrapper_aes256_encrypt(c->aeswrapper);
      c->buffer = c->aeswrapper->hash;
}

void conn_wrapper_aes256_decrypt(Connection *c)
{
      c->aeswrapper->hash = c->buffer;
      c->aeswrapper->length = c->len;

      c->len = wrapper_aes256_decrypt(c->aeswrapper);
      c->buffer = c->aeswrapper->plain;
}



