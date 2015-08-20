/* bcal_rc5.c */
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
 * \file     bcal_rc5.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-01-09
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "blockcipher_descriptor.h"
#include "rc5.h"
#include "keysize_descriptor.h"

#define RC5_ROUNDS 12

const char rc5_str[]   PROGMEM = "RC5";

const uint8_t rc5_keysize_desc[] PROGMEM = { KS_TYPE_RANGE, KS_INT(1), KS_INT(2040), 
                                                KS_TYPE_TERMINATOR    };

static
void rc5_dummy_init(void* key, uint16_t keysize_b, void* ctx){
	rc5_init(key, keysize_b, RC5_ROUNDS, ctx);
}

const bcdesc_t rc5_desc PROGMEM = {
	BCDESC_TYPE_BLOCKCIPHER,
	BC_INIT_TYPE_2,
	rc5_str,
	sizeof(rc5_ctx_t),
	128,
	{(void_fpt)rc5_dummy_init},
	{(void_fpt)rc5_enc},
	{(void_fpt)rc5_dec},
	(bc_free_fpt)rc5_free,
	rc5_keysize_desc
};


