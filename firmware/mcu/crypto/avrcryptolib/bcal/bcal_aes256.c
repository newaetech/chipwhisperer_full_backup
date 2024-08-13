/* bcal_aes256.c */
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
 * \file     bcal_aes256.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-01-08
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "blockcipher_descriptor.h"
#include "aes.h"
#include "aes256_enc.h"
#include "aes256_dec.h"
#include "aes_keyschedule.h"
#include "keysize_descriptor.h"

const char aes256_str[]   PROGMEM = "AES-256";

const uint8_t aes256_keysize_desc[] PROGMEM = { KS_TYPE_LIST, 1, KS_INT(256), 
                                                KS_TYPE_TERMINATOR    };

const bcdesc_t aes256_desc PROGMEM = {
	BCDESC_TYPE_BLOCKCIPHER,
	BC_INIT_TYPE_1,
	aes256_str,
	sizeof(aes256_ctx_t),
	128,
	{(void_fpt)aes256_init},
	{(void_fpt)aes256_enc},
	{(void_fpt)aes256_dec},
	(bc_free_fpt)NULL,
	aes256_keysize_desc
};


