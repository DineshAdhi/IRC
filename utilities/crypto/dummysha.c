#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define CHUNK_SIZE 64
#define INT_LEN 8
#define TOTAL_LEN_LEN 8

typedef struct {
    uint8_t *p;
    size_t len;
    uint64_t total_len;
    int single_bit_delivered;
    int total_delivered;
} SHA256Ctx;

static const uint32_t k[] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static inline uint32_t right_rot(uint32_t value, unsigned int count)
{
	return value >> count | value << (32 - count);
}

void init_sha256ctx(SHA256Ctx *ctx, void *input, int len)
{
        ctx->total_len = len;
        ctx->p = input;
        ctx->len = len;
        ctx->single_bit_delivered = 0;
        ctx->total_delivered = 0;
}

int getChunk(uint8_t *chunk, SHA256Ctx *ctx)
{
        int i;
        size_t space_left_in_chunk;

        if(ctx->total_delivered == 1)
        {
                return 0;
        }

        if(ctx->len >= CHUNK_SIZE)
        {
                memcpy(chunk, ctx->p, CHUNK_SIZE);
                ctx->p +=  CHUNK_SIZE;
                ctx->len -= CHUNK_SIZE;
                return 1;
        }

        memcpy(chunk, ctx->p, ctx->len);
        chunk += ctx->len;
        space_left_in_chunk = CHUNK_SIZE - ctx->len;
        ctx->p += ctx->len;
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
                chunk = chunk + left;    

                len = len * 8;

                chunk[0] = (len >> 56); 
                chunk[1] = (len >> 48); 
                chunk[2] = (len >> 40); 
                chunk[3] = (len >> 32); 
                chunk[4] = (len >> 24); 
                chunk[5] = (len >> 16); 
                chunk[6] = (len >> 8); 
                chunk[7] = (len >> 0); 

                ctx->total_delivered = 1;
        }
        else 
        {
                memset(chunk, 0x00, space_left_in_chunk);
        }
        
        return 1;
}


void sha256(uint8_t *hash, uint8_t *ptr, size_t size)
{
    uint32_t h[] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
	int i, j;

	uint8_t chunk[64];

	SHA256Ctx ctx;

	init_sha256ctx(&ctx, ptr, size);

	while (getChunk(chunk, &ctx)) {
		uint32_t ah[8];
		
		uint32_t w[64];
		const uint8_t *p = chunk;

		memset(w, 0x00, sizeof w);
		
        for (i = 0; i < 16; i++) {
			w[i] = (uint32_t) p[0] << 24 | (uint32_t) p[1] << 16 |
				(uint32_t) p[2] << 8 | (uint32_t) p[3];
			p += 4;
		}

		for (i = 16; i < 64; i++) {
			const uint32_t s0 = right_rot(w[i - 15], 7) ^ right_rot(w[i - 15], 18) ^ (w[i - 15] >> 3);
			const uint32_t s1 = right_rot(w[i - 2], 17) ^ right_rot(w[i - 2], 19) ^ (w[i - 2] >> 10);
			w[i] = w[i - 16] + s0 + w[i - 7] + s1;
		}
		
		for (i = 0; i < 8; i++)
			ah[i] = h[i];

		for (i = 0; i < 64; i++) {
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
			h[i] += ah[i];
	}

	for (i = 0, j = 0; i < 8; i++)
	{
		hash[j++] = (uint8_t) (h[i] >> 24);
		hash[j++] = (uint8_t) (h[i] >> 16);
		hash[j++] = (uint8_t) (h[i] >> 8);
		hash[j++] = (uint8_t) h[i];
	}
}

struct Data {
    int a;
    int b;
};



int main()
{
    int i;

    // struct Data d;
    // d.a = 10;
    // d.b = 100;

    char *name = "Dinesh";
    size_t len = strlen(name);
    uint8_t hash[32] = {};

    sha256(hash, (uint8_t *) name, len);

    char *str = (char *) calloc(65, sizeof(str));
    int itr = 0;

    for(i=0; i<32; i++)
    {
        sprintf(&str[itr++], "%01X", hash[i] & 0xF0);
        sprintf(&str[itr++], "%01X", hash[i] & 0x0F);
    }

    printf("\n\n%s", str);
}
