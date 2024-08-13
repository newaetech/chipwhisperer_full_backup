/* twister-small.h */

#ifndef TWISTER_SMALL_H_
#define TWISTER_SMALL_H_

#include <stdint.h>
#include "twister.h"

typedef uint8_t twister256_hash_t[256/8];
typedef uint8_t twister224_hash_t[224/8];

typedef twister_state_t twister256_ctx_t;
typedef twister_state_t twister224_ctx_t;

void twister_small_init(twister_state_t* ctx, uint16_t hashsize_b);
void twister_small_lastBlock(twister_state_t* ctx, const void* msg, uint16_t length_b);
void twister_small_ctx2hash(void* dest, twister_state_t* ctx, uint16_t hashsize_b);
void twister_small_nextBlock(twister_state_t* ctx, const void* msg);


void twister256_init(twister256_ctx_t* ctx);
void twister256_nextBlock(twister256_ctx_t* ctx, const void* msg);
void twister256_lastBlock(twister256_ctx_t* ctx, const void* msg, uint16_t length_b);
void twister256_ctx2hash(void* dest, twister_state_t* ctx);
void twister256(void* dest, const void* msg, uint32_t msg_length_b);

void twister224_init(twister224_ctx_t* ctx);
void twister224_nextBlock(twister224_ctx_t* ctx, const void* msg);
void twister224_lastBlock(twister224_ctx_t* ctx, const void* msg, uint16_t length_b);
void twister224_ctx2hash(void* dest, twister224_ctx_t* ctx);
void twister224(void* dest, const void* msg, uint32_t msg_length_b);

#endif /* TWISTER_SMALL_H_ */
