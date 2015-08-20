#include "noekeon.h"
#include "omac_noekeon.h"
#include "memxor.h"
#include <string.h>
#include <stdint.h>


void omac_noekeon_init(omac_noekeon_ctx_t* ctx){
	memset(ctx, 0, 16);
}


void omac_noekeon_tweak(uint8_t t, const void* key, omac_noekeon_ctx_t* ctx){
	*ctx[15] = t;
	noekeon_enc(ctx, key);
}

void omac_noekeon_next(const void* buffer, const void* key, omac_noekeon_ctx_t* ctx){
	memxor(ctx, buffer, 16);
	noekeon_enc(ctx, key);
}

static
void omac_noekeon_comppad(uint8_t* pad, const void* key, uint8_t length_b){
	uint8_t c1,c2,r,j;
	memset(pad, 0, 16);
	noekeon_enc(pad, key);
	r=(length_b==128)?1:2;
	for(;r!=0;--r){
		c1=0;
		for(j=0;j<16;++j){
			c2 = c1;
			c1 = (pad[15-j])>>7;
			pad[15-j] = ((pad[15-j])<<1) | c2;
		}
		if(c1){
			pad[15] ^= 0x87;
		}
	}
	if(length_b<128){
		pad[(length_b)/8] ^= 0x80 >> (length_b%8);
	}
}

void omac_noekeon_last(const void* buffer, uint8_t length_b, const void* key, omac_noekeon_ctx_t* ctx){
	while(length_b>128){
		omac_noekeon_next(buffer, key, ctx);
		buffer = (uint8_t*)buffer +16;
		length_b -= 128;
	}
	uint8_t pad[16];
	omac_noekeon_comppad(pad, key, length_b);
	memxor(pad, buffer, (length_b+7)/8);
	omac_noekeon_next(pad, key, ctx);
}


void omac_noekeon(void* dest, const void* msg, uint16_t msglength_b,
                  const void* key, uint8_t t){
	omac_noekeon_init(dest);
	if(t!=0xff)
		omac_noekeon_tweak(t,key,dest);
	while(msglength_b>128){
		omac_noekeon_next(msg, key, dest);
		msg = (uint8_t*)msg +16;
		msglength_b -= 128;
	}
	omac_noekeon_last(msg, msglength_b, key, dest);                  	
}





