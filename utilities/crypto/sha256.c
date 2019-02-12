#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"../../include/sha256.h"

void sha256ctx_int(SHA256Ctx *ctx, void *input, size_t len)
{
    ctx->dataptr = input;
    ctx->len = len;
    ctx->total_len = len;
    ctx->single_bit_delivered = 0;
    ctx->total_data_delivered = 0;
}

int getchunck(uint8_t *chunk, SHA256Ctx *ctx)
{
        if(ctx->total_data_delivered == 1)
        {
                return 0;
        }

        if(ctx->len >= CHUNK_SIZE)
        {
            memcpy(chunk, ctx->dataptr, CHUNK_SIZE);
            ctx->dataptr += CHUNK_SIZE;
            ctx->len -= CHUNK_SIZE;
            return 1;
        }

        size_t space_left_in_chunk;

        memcpy(chunk, ctx->dataptr, ctx->len);
        chunk += ctx->len;
        space_left_in_chunk = CHUNK_SIZE - ctx->len;
        ctx->dataptr += ctx->len;
        ctx->len = 0;

        if(!ctx->single_bit_delivered)
        {
                *chunk = 0x80;
                chunk++;
                space_left_in_chunk -= 1;
                ctx->single_bit_delivered = 1;
        }

        if(space_left_in_chunk >= INT_LEN)
        {
                size_t left = space_left_in_chunk - INT_LEN;
                size_t len = ctx->total_len;

                memset(chunk, 0x00, left);
                chunk += left;

                len = len * 8;

                chunk[0] = (len >> 56); 
                chunk[1] = (len >> 48); 
                chunk[2] = (len >> 40); 
                chunk[3] = (len >> 32); 
                chunk[4] = (len >> 24); 
                chunk[5] = (len >> 16); 
                chunk[6] = (len >> 8); 
                chunk[7] = (len >> 0); 

                ctx->total_data_delivered = 1;
        }
        else
        {
                memset(chunk, 0x00, space_left_in_chunk);
        }

        return 1;
}

void calc_hash_sha256(uint8_t *hash, uint8_t *input, size_t len)
{

        uint8_t chunk[64];

        SHA256Ctx ctx; 
        sha256ctx_int(&ctx, input, len);

        while(getchunck(chunk, &ctx))
        {
                uint32_t ah[8];
                uint32_t w[64];
                const uint8_t *p = chunk;

                memset(w, 0x00, sizeof w);

                
                for (i = 0; i < 16; i++) 
                {
                    w[i] = (uint32_t) p[0] << 24 | (uint32_t) p[1] << 16 |
                        (uint32_t) p[2] << 8 | (uint32_t) p[3];
                    p += 4;
                }

                for (i = 16; i < 64; i++) 
                {
                    const uint32_t s0 = right_rot(w[i - 15], 7) ^ right_rot(w[i - 15], 18) ^ (w[i - 15] >> 3);
                    const uint32_t s1 = right_rot(w[i - 2], 17) ^ right_rot(w[i - 2], 19) ^ (w[i - 2] >> 10);
                    w[i] = w[i - 16] + s0 + w[i - 7] + s1;
                }
                
                for (i = 0; i < 8; i++)
                    ah[i] = h[i];

                for (i = 0; i < 64; i++) 
                {
                        const uint32_t s1 = right_rot(ah[4], 6) ^ right_rot(ah[4], 11) ^ right_rot(ah[4], 25);
                        const uint32_t ch = (ah[4] & ah[5]) ^ (~ah[4] & ah[6]);
                        const uint32_t temp1 = ah[7] + s1 + ch + k[i] + w[i];
                        const uint32_t s0 = right_rot(ah[0], 2) ^ right_rot(ah[0], 13) ^ right_rot(ah[0], 22);
                        const uint32_t maj = (ah[0] & ah[1]) ^ (ah[0] & ah[2]) ^ (ah[1] & ah[2]);
                        const uint32_t temp2 = s0 + maj;

                        ah[7] = ah[6];
                        ah[6] = ah[5];
                        ah[5] = ah[4];
                        ah[4] = ah[3] + temp1;
                        ah[3] = ah[2];
                        ah[2] = ah[1];
                        ah[1] = ah[0];
                        ah[0] = temp1 + temp2;
                }

                for (i = 0; i < 8; i++)
                {
                    h[i] += ah[i];
                }
        }

        for (i = 0, j = 0; i < 8; i++)
        {
                hash[j++] = (uint8_t) (h[i] >> 24);
                hash[j++] = (uint8_t) (h[i] >> 16);
                hash[j++] = (uint8_t) (h[i] >> 8);
                hash[j++] = (uint8_t) h[i];
        }
}


void sha256(char *encoded, char *value)
{
    size_t len = strlen(value);

    uint8_t hash[32] = {};

    calc_hash_sha256(hash, (uint8_t *)value, len);

    int itr = 0;

    for(i=0; i<32; i++)
    {
            sprintf(&encoded[itr++], "%01X", hash[i] & 0xF0);
            sprintf(&encoded[itr++], "%01X", hash[i] & 0x0F);
    }

    printf("%s\n", encoded);
}