/* hfal-nessie.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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
/*
 * \file    hfal-nessie.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-05-10
 * \license GPLv3 or later
 * 
 */

#include "nessie_hash_test.h"
#include "hashfunction_descriptor.h"
#include <stdint.h>
#include <avr/pgmspace.h>

void hfal_nessie(const hfdesc_t* hd){
	if(pgm_read_byte(&(hd->type))!=HFDESC_TYPE_HASHFUNCTION)
		return;
	char name[1+strlen_P((void*)pgm_read_word(&(hd->name)))];
	strcpy_P(name, (void*)pgm_read_word(&(hd->name)));
	
	nessie_hash_ctx.hashsize_b  = pgm_read_word(&(hd->hashsize_b));
	nessie_hash_ctx.name = name;
	nessie_hash_ctx.blocksize_B = pgm_read_word(&(hd->blocksize_b))/8;
	nessie_hash_ctx.ctx_size_B  = pgm_read_word(&(hd->ctxsize_B));
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)pgm_read_word(&(hd->init));
	nessie_hash_ctx.hash_next = (nessie_hash_next_fpt)pgm_read_word(&(hd->nextBlock));
	nessie_hash_ctx.hash_last = (nessie_hash_last_fpt)pgm_read_word(&(hd->lastBlock));
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)pgm_read_word(&(hd->ctx2hash));
	
	nessie_hash_run();
}

void hfal_nessie_multiple(const hfdesc_t* const* hd_list){
	const hfdesc_t* hd;
	for(;;){
		hd = (void*)pgm_read_word(hd_list);
		if(!hd)
			return;
		hfal_nessie(hd);
		hd_list = (void*)((uint8_t*)hd_list + 2);
	}
}

