/* bcal_threefish256.c */
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
 * \file     bcal_threefish256.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte
 * \date     2010-02-20
 * \license  GPLv3 or later
 *
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "blockcipher_descriptor.h"
#include "threefish.h"
#include "keysize_descriptor.h"

const char threefish256_str[]   PROGMEM = "Threefish-256";

const uint8_t threefish256_keysize_desc[] PROGMEM = { KS_TYPE_LIST, 1, KS_INT(256),
                                                KS_TYPE_TERMINATOR    };

static void threefish256_dummy_init(void* key, void* ctx){
	threefish256_init(key, NULL, ctx);
}

const bcdesc_t threefish256_desc PROGMEM = {
	BCDESC_TYPE_BLOCKCIPHER,
	BC_INIT_TYPE_1,
	threefish256_str,
	sizeof(threefish256_ctx_t),
	256,
	{(void_fpt)threefish256_dummy_init},
	{(void_fpt)threefish256_enc},
	{(void_fpt)threefish256_dec},
	(bc_free_fpt)NULL,
	threefish256_keysize_desc
};


