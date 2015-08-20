/* bcal-nessie.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)

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

#include "nessie_bc_test.h"
#include "blockcipher_descriptor.h"
#include "keysize_descriptor.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>


void(*bcal_nessie_dummy_init_fpt)(const void* key, void* ctx)=NULL;

void bcal_nessie_dummy_init(const void* key, uint16_t keysize, void* ctx){
	if(bcal_nessie_dummy_init_fpt){
		bcal_nessie_dummy_init_fpt(key, ctx);
	}else{
		memcpy(ctx, key, (keysize+7)/8);
	}
}

void bcal_nessie(const bcdesc_t* bcd){
	if(pgm_read_byte(&(bcd->type))!=BCDESC_TYPE_BLOCKCIPHER)
		return;
	char name[1+strlen_P((void*)pgm_read_word(&(bcd->name)))];
	strcpy_P(name, (void*)pgm_read_word(&(bcd->name)));
	nessie_bc_init();

	nessie_bc_ctx.blocksize_B = (pgm_read_word(&(bcd->blocksize_b))+7)/8;
	nessie_bc_ctx.name        = name;
	nessie_bc_ctx.ctx_size_B  = pgm_read_word(&(bcd->ctxsize_B));
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)pgm_read_word(&(bcd->enc));
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)pgm_read_word(&(bcd->dec));
	nessie_bc_ctx.cipher_free = (nessie_bc_free_fpt)pgm_read_word(&(bcd->free));
	if((pgm_read_byte(&(bcd->flags))&BC_INIT_TYPE)==BC_INIT_TYPE_2){
		nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)pgm_read_word(&(bcd->init));
	}else{
		bcal_nessie_dummy_init_fpt = (void(*)(const void*,void*))pgm_read_word(&(bcd->init));
		nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)bcal_nessie_dummy_init;
	}

	uint16_t *keysize_list=NULL;
	uint16_t items,i;
	items = get_keysizes((PGM_VOID_P)pgm_read_word(&(bcd->valid_keysize_desc)), &keysize_list);
	if(items){
		for(i=0; i<items; ++i){
			nessie_bc_ctx.keysize_b   = keysize_list[i];
			nessie_bc_run();
		}
		free(keysize_list);
	}

}

void bcal_nessie_multiple(const bcdesc_t* const* bcd_list){
	const bcdesc_t* bcd;
	for(;;){
		bcd = (void*)pgm_read_word(bcd_list);
		if(!bcd)
			return;
		bcal_nessie(bcd);
		bcd_list = (void*)((uint8_t*)bcd_list + 2);
	}
}

