#ifndef CAST6_H_
#define CAST6_H_

#include <stdint.h>

#define CAST6_ROUNDS 12

/* size of this is 222 byte (HUGE) */
typedef struct cast6_ctx_st{
	uint32_t	km[12][4];
	uint8_t		krx[4*12*5/8]; /* these are packed */
} cast6_ctx_t;



void cast6_enc(void* buffer, const cast6_ctx_t* ctx);
void cast6_dec(void* buffer, const cast6_ctx_t* ctx);
void cast6_init(const void* key, uint16_t keysize_b, cast6_ctx_t* ctx);


#endif /*CAST6_H_*/

