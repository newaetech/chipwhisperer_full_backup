/* twister-large.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <string.h>
#include "memxor.h"
#include "twister.h"
#include "twister-large.h"

/*********************************************************************/

static
void checksum_update(twister_large_ctx_t* ctx, uint8_t col){
	uint8_t i, col2;
	uint8_t carry = 0;
	int sum = 0;
	
	col2 = (col+1)%8; 
	
	for( i=0; i<8; i++ ) 
	{
		sum =  (int) ctx->checksum[col2][7-i] 
		     + (int) ctx->state.s[7-i][0] /* col or 0 ???*/ 
		     + carry;
		ctx->checksum[col][7-i] ^= (uint8_t)sum;
		carry = sum>>8;

 	}
}

/*********************************************************************/

void twister_large_init(twister_large_ctx_t* ctx, uint16_t hashsize_b){
	memset(ctx->state.s,  0, 64);
	memset(ctx->checksum, 0, 64);
	ctx->state.counter=0xffffffffffffffffLL;
	ctx->state.s[0][7] = hashsize_b>>8;
	ctx->state.s[1][7] = hashsize_b&0xff;
	ctx->state.length_counter_b = 0;
}

/*********************************************************************/

void twister_large_nextBlock(twister_large_ctx_t* ctx, const void* msg){
	uint8_t tmp[8][8];

	/* 1st maxi round */
	memcpy(tmp, ctx->state.s, 64);	
	checksum_update(ctx, 0);
	twister_mini_round(&(ctx->state), msg);
	msg = (uint8_t*)msg + 8;

	checksum_update(ctx, 1);
	twister_mini_round(&(ctx->state), msg);
	msg = (uint8_t*)msg + 8;
	
	checksum_update(ctx, 2);
	twister_mini_round(&(ctx->state), msg);
	msg = (uint8_t*)msg + 8;
	memxor(ctx->state.s, tmp, 64);

	/* 2nd maxi round */
	memcpy(tmp, ctx->state.s, 64);	
	checksum_update(ctx, 3);
	twister_mini_round(&(ctx->state), msg);
	msg = (uint8_t*)msg + 8;

	twister_blank_round(&(ctx->state));
	
	checksum_update(ctx, 4);
	twister_mini_round(&(ctx->state), msg);
	msg = (uint8_t*)msg + 8;
	memxor(ctx->state.s, tmp, 64);

	/* 3rd maxi round */
	memcpy(tmp, ctx->state.s, 64);	
	checksum_update(ctx, 5);
	twister_mini_round(&(ctx->state), msg);
	msg = (uint8_t*)msg + 8;

	checksum_update(ctx, 6);
	twister_mini_round(&(ctx->state), msg);
	msg = (uint8_t*)msg + 8;

	checksum_update(ctx, 7);
	twister_mini_round(&(ctx->state), msg);

	twister_blank_round(&(ctx->state));	
	memxor(ctx->state.s, tmp, 64);
	ctx->state.length_counter_b += 512;
}

/*********************************************************************/

void twister_inject_chksum(twister_large_ctx_t* ctx, uint8_t col){
	uint8_t i=7;
	do{
		ctx->state.s[7][i] ^= ctx->checksum[col][i];

	}while(i--);
	twister_blank_round(&ctx->state);
}

/*********************************************************************/

void twister_large_lastBlock(twister_large_ctx_t* ctx, const void* msg, uint16_t length_b){
	uint8_t tmp[64];	
	while(length_b>=512){
		twister_large_nextBlock(ctx, msg);
		msg = ((uint8_t*)msg)+64;
		length_b -= 512;
	}
	memset(tmp, 0, 64);
	memcpy(tmp, msg, (length_b+7)/8);
	tmp[length_b/8] |= 0x80 >> (length_b&0x07);
	twister_large_nextBlock(ctx, tmp);
	ctx->state.length_counter_b -= 512 - length_b;
	twister_mini_round(&(ctx->state), &(ctx->state.length_counter_b));

	memcpy(tmp, ctx->state.s, 64);
	twister_inject_chksum(ctx, 0);
	twister_inject_chksum(ctx, 1);
	twister_inject_chksum(ctx, 2);
	memxor(ctx->state.s, tmp, 64);

	memcpy(tmp, ctx->state.s, 64);
	twister_inject_chksum(ctx, 3);
	twister_inject_chksum(ctx, 4);
	twister_inject_chksum(ctx, 5);
	memxor(ctx->state.s, tmp, 64);

	memcpy(tmp, ctx->state.s, 64);
	twister_inject_chksum(ctx, 6);
	twister_inject_chksum(ctx, 7);
	twister_blank_round(&(ctx->state));
	memxor(ctx->state.s, tmp, 64);

}

/*********************************************************************/

void twister_large_ctx2hash(void* dest, twister_large_ctx_t* ctx, uint16_t hashsize_b){
	twister_ctx2hash(dest, &(ctx->state), hashsize_b);
}

/*********************************************************************/
/*********************************************************************/

void twister384_init(twister384_ctx_t* ctx){
	twister_large_init(ctx, 384);
}

/*********************************************************************/

void twister384_nextBlock(twister384_ctx_t* ctx, const void* msg){
	twister_large_nextBlock(ctx, msg);
}

/*********************************************************************/

void twister384_lastBlock(twister384_ctx_t* ctx, const void* msg, uint16_t length_b){
	twister_large_lastBlock(ctx, msg, length_b);
}

/*********************************************************************/

void twister384_ctx2hash(void* dest, twister384_ctx_t* ctx){
	twister_large_ctx2hash(dest, ctx, 384);
}

/*********************************************************************/

void twister384(void* dest, const void* msg, uint32_t msg_length_b){
	twister_large_ctx_t ctx;
	twister_large_init(&ctx, 384);
	while(msg_length_b >=512){
		twister_large_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg + 512/8;
		msg_length_b -= 512;
	}
	twister_large_lastBlock(&ctx, msg, msg_length_b);
	twister_large_ctx2hash(dest, &ctx, 384);
}

/*********************************************************************/
/*********************************************************************/


void twister512_init(twister512_ctx_t* ctx){
	twister_large_init(ctx, 512);
}

/*********************************************************************/

void twister512_nextBlock(twister512_ctx_t* ctx, const void* msg){
	twister_large_nextBlock(ctx, msg);
}

/*********************************************************************/

void twister512_lastBlock(twister512_ctx_t* ctx, const void* msg, uint16_t length_b){
	twister_large_lastBlock(ctx, msg, length_b);
}

/*********************************************************************/

void twister512_ctx2hash(void* dest, twister512_ctx_t* ctx){
	twister_large_ctx2hash(dest, ctx, 512);
}

/*********************************************************************/

void twister512(void* dest, const void* msg, uint32_t msg_length_b){
	twister_large_ctx_t ctx;
	twister_large_init(&ctx, 512);
	while(msg_length_b >=512){
		twister_large_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg + 512/8;
		msg_length_b -= 512;
	}
	twister_large_lastBlock(&ctx, msg, msg_length_b);
	twister_large_ctx2hash(dest, &ctx, 512);
}



