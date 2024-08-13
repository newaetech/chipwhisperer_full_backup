/* twister-small.c */

#include <stdint.h>
#include <string.h>
#include "memxor.h"
#include "twister-small.h"

/*********************************************************************/

void twister_small_init(twister_state_t* ctx, uint16_t hashsize_b){
	memset(ctx->s, 0, 64);
	ctx->counter=0xffffffffffffffffLL;
	ctx->s[0][7] = hashsize_b>>8;
	ctx->s[1][7] = hashsize_b&0xff;
	ctx->length_counter_b = 0;
}

/*********************************************************************/

void twister_small_nextBlock(twister_state_t* ctx, const void* msg){
	uint8_t tmp[8][8];
	/* round 1 */
	memcpy(tmp, ctx->s, 64);
	twister_mini_round(ctx, msg);
	msg = ((uint8_t*)msg) + 8;
	twister_mini_round(ctx, msg);
	msg = ((uint8_t*)msg) + 8;
	twister_mini_round(ctx, msg);
	msg = ((uint8_t*)msg) + 8;
	memxor(ctx->s, tmp, 64);

	/* round 2 */
	memcpy(tmp, ctx->s, 64);
	twister_mini_round(ctx, msg);
	msg = ((uint8_t*)msg) + 8;
	twister_mini_round(ctx, msg);
	msg = ((uint8_t*)msg) + 8;
	twister_mini_round(ctx, msg);
	msg = ((uint8_t*)msg) + 8;
	memxor(ctx->s, tmp, 64);

	/* round 3 */
	memcpy(tmp, ctx->s, 64);
	twister_mini_round(ctx, msg);
	msg = ((uint8_t*)msg) + 8;
	twister_mini_round(ctx, msg);
	twister_blank_round(ctx);
	memxor(ctx->s, tmp, 64);
	ctx->length_counter_b += 512;
}

/*********************************************************************/

void twister_small_lastBlock(twister_state_t* ctx, const void* msg, uint16_t length_b){
	uint8_t tmp[64];	
	while(length_b>=512){
		twister_small_nextBlock(ctx, msg);
		msg = ((uint8_t*)msg)+64;
		length_b -= 512;
	}
	memset(tmp, 0, 64);
	memcpy(tmp, msg, (length_b+7)/8);
	tmp[length_b/8] |= 0x80 >> (length_b&0x07);
	twister_small_nextBlock(ctx, tmp);
	ctx->length_counter_b -= 512 - length_b;

	twister_mini_round(ctx, &(ctx->length_counter_b));
	twister_blank_round(ctx);
}

/*********************************************************************/

void twister_small_ctx2hash(void* dest, twister_state_t* ctx, uint16_t hashsize_b){
	twister_ctx2hash(dest, ctx, hashsize_b);
}

/*********************************************************************/
/*********************************************************************/

#ifndef NO_TWISTER_256

void twister256_init(twister256_ctx_t* ctx){
	twister_small_init(ctx, 256);
}

/*********************************************************************/

void twister256_nextBlock(twister256_ctx_t* ctx, const void* msg){
	twister_small_nextBlock(ctx, msg);
}

/*********************************************************************/

void twister256_lastBlock(twister256_ctx_t* ctx, const void* msg, uint16_t length_b){
	twister_small_lastBlock(ctx, msg, length_b);
}

/*********************************************************************/

void twister256_ctx2hash(void* dest, twister256_ctx_t* ctx){
	twister_ctx2hash(dest, ctx, 256);
}

/*********************************************************************/

void twister256(void* dest, const void* msg, uint32_t msg_length_b){
	twister_state_t ctx;
	twister_small_init(&ctx, 256);
	while(msg_length_b >=512){
		twister_small_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg + 512/8;
		msg_length_b -= 512;
	}
	twister_small_lastBlock(&ctx, msg, msg_length_b);
	twister_ctx2hash(dest, &ctx, 256);
}

/*********************************************************************/

#endif

/*********************************************************************/
/*********************************************************************/

#ifndef NO_TWISTER_224

void twister224_init(twister224_ctx_t* ctx){
	twister_small_init(ctx, 224);
}

/*********************************************************************/

void twister224_nextBlock(twister224_ctx_t* ctx, const void* msg){
	twister_small_nextBlock(ctx, msg);
}

/*********************************************************************/

void twister224_lastBlock(twister224_ctx_t* ctx, const void* msg, uint16_t length_b){
	twister_small_lastBlock(ctx, msg, length_b);
}

/*********************************************************************/

void twister224_ctx2hash(void* dest, twister224_ctx_t* ctx){
	twister_ctx2hash(dest, ctx, 224);
}

/*********************************************************************/

void twister224(void* dest, const void* msg, uint32_t msg_length_b){
	twister_state_t ctx;
	twister_small_init(&ctx, 224);
	while(msg_length_b >=512){
		twister_small_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg + 512/8;
		msg_length_b -= 512;
	}
	twister_small_lastBlock(&ctx, msg, msg_length_b);
	twister_ctx2hash(dest, &ctx, 224);
}

#endif

