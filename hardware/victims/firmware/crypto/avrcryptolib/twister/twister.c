/* twister.c */
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

#include "config.h"
#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "twister.h"
#include "twister_tables.h"
#include "memxor.h"

//#ifndef TWISTER_MUL_TABLE
# include "gf256mul/gf256mul.h"
//#endif

#define MDS(a,b)  pgm_read_byte(&(twister_mds[(a)][(b)]))

//#ifdef TWISTER_MUL_TABLE
//# define MULT(a,b) pgm_read_byte(&(twister_multab[(a)][(b)]))
//#else
# define MULT(a,b) gf256mul((a),(b), 0x4D)
//#endif

void twister_blank_round(twister_state_t* ctx){
	uint8_t i,j,k;
	uint8_t tmp[8][8];
	/* add twist counter */
	for(i=0; i<8; ++i){
		ctx->s[i][1] ^= ((uint8_t*)&(ctx->counter))[7-i];
	}
	ctx->counter--;
	/* sub bytes */
	for(i=0; i<8; ++i){
		for(j=0;j<8;++j){
			tmp[i][j] = pgm_read_byte(twister_sbox+ctx->s[i][j]);
		}
	}
	/* mix columns with integrates shift rows */
	for( i=0; i<8; i++ ){
		// multiply with mds matrix
		for( j=0; j<8; j++ ){
			k=(i+1)&7;
			ctx->s[j][i] =
				MULT( MDS(j,0), (tmp[0][i]) );
			ctx->s[j][i] ^=	 
				MULT( MDS(j,1), (tmp[1][k]) );
			ctx->s[j][i] ^=	 
				MULT( MDS(j,2), (tmp[2][((++k)&7)]) );
			ctx->s[j][i] ^=	 
				MULT( MDS(j,3), (tmp[3][((++k)&7)]) );
			ctx->s[j][i] ^=	 
				MULT( MDS(j,4), (tmp[4][((++k)&7)]) );
			ctx->s[j][i] ^=	 
				MULT( MDS(j,5), (tmp[5][((++k)&7)]) );
			ctx->s[j][i] ^=	 
				MULT( MDS(j,6), (tmp[6][((++k)&7)]) );
			ctx->s[j][i] ^=	 
				MULT( MDS(j,7), (tmp[7][((++k)&7)]) );
				
		}	
	}
}
void twister_mini_round(twister_state_t* ctx, const void* msg){
	/* inject message */
	uint8_t i;
	for(i=0; i<8; ++i){
		ctx->s[7][7-i] ^= *((uint8_t*)msg);
		msg = (uint8_t*)msg +1;	
	}
	twister_blank_round(ctx);
}

void twister_ctx2hash(void* dest, twister_state_t* ctx, uint16_t hashsize_b){
	uint8_t tmp[8][8];
	uint8_t j;
	uint16_t i=hashsize_b;
	while(i>=64){
		i-=64;
		memcpy(tmp,ctx->s, 64);
		twister_blank_round(ctx);
		memxor(ctx->s, tmp, 64);
		twister_blank_round(ctx);
		for(j=0; j<8; ++j){
			*((uint8_t*)dest) = ctx->s[7-j][0] ^ tmp[7-j][0];
			dest = (uint8_t*)dest + 1;
		}
	}
	if(i>=32){
		memcpy(tmp,ctx->s, 64);
		twister_blank_round(ctx);
		memxor(ctx->s, tmp, 64);
		twister_blank_round(ctx);
		for(j=0; j<4; ++j){
			*((uint8_t*)dest) = ctx->s[7-j][0] ^ tmp[7-j][0];
			dest = (uint8_t*)dest + 1;
		}
	}
}


