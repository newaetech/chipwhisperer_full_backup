/* bcal_tdes2.c */
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
/**
 * \file     bcal_tdes.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2010-02-02
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>
#include "blockcipher_descriptor.h"
#include "des.h"
#include "keysize_descriptor.h"

const char tdes2_str[]   PROGMEM = "TDES-2";

const uint8_t tdes2_keysize_desc[] PROGMEM = { KS_TYPE_LIST, 1, KS_INT(128),
                                                KS_TYPE_TERMINATOR    };

static
void tdes_dummy_enc(void* block, void* key){
	tdes_enc(block, block, key);
}

static
void tdes_dummy_dec(void* block, void* key){
	tdes_dec(block, block, key);
}

static
void tdes2_init(void* key, void* ctx){
	memcpy(ctx, key, 16);
	memcpy((uint8_t*)ctx+16, key, 8);
}



const bcdesc_t tdes2_desc PROGMEM = {
	BCDESC_TYPE_BLOCKCIPHER,
	BC_INIT_TYPE_1,
	tdes2_str,
	24,
	64,
	{(void_fpt)tdes2_init},
	{(void_fpt)tdes_dummy_enc},
	{(void_fpt)tdes_dummy_dec},
	(bc_free_fpt)NULL,
	tdes2_keysize_desc
};


