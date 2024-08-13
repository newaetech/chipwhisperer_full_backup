/* bcal_cscipher.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010  Daniel Otte (daniel.otte@rub.de)

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
 * \file     bcal_cscipher.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2010-12-17
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "blockcipher_descriptor.h"
#include "cscipher.h"
#include "keysize_descriptor.h"


const char cscipher_str[]   PROGMEM = "CS-Cipher";

const uint8_t cscipher_keysize_desc[] PROGMEM = { KS_TYPE_LIST, 1, KS_INT(128),
                                                 KS_TYPE_TERMINATOR    };

const bcdesc_t cscipher_desc PROGMEM = {
	BCDESC_TYPE_BLOCKCIPHER,
	BC_INIT_TYPE_1,
	cscipher_str,
	sizeof(cscipher_ctx_t),
	64,
	{(void_fpt)cscipher_init},
	{(void_fpt)cscipher_enc},
	{(void_fpt)cscipher_dec},
	(bc_free_fpt)NULL,
	cscipher_keysize_desc
};


