/* bcal_threefish512.c */
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
 * \file     bcal_threefish512.c
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

const char threefish512_str[]   PROGMEM = "Threefish-512";

const uint8_t threefish512_keysize_desc[] PROGMEM = { KS_TYPE_LIST, 1, KS_INT(512),
                                                KS_TYPE_TERMINATOR    };

static void threefish512_dummy_init(void* key, void* ctx){
	threefish512_init(key, NULL, ctx);
}

const bcdesc_t threefish512_desc PROGMEM = {
	BCDESC_TYPE_BLOCKCIPHER,
	BC_INIT_TYPE_1,
	threefish512_str,
	sizeof(threefish512_ctx_t),
	512,
	{(void_fpt)threefish512_dummy_init},
	{(void_fpt)threefish512_enc},
	{(void_fpt)threefish512_dec},
	(bc_free_fpt)NULL,
	threefish512_keysize_desc
};


