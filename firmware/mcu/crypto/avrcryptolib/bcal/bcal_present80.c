/* bcal_present.c */
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
 * \file     bcal_present.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-01-09
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "blockcipher_descriptor.h"
#include "present80.h"
#include "keysize_descriptor.h"

const char present80_str[]   PROGMEM = "Present80";

const uint8_t present80_keysize_desc[] PROGMEM = { KS_TYPE_LIST, 1, KS_INT(80),
                                                KS_TYPE_TERMINATOR    };

const bcdesc_t present80_desc PROGMEM = {
	BCDESC_TYPE_BLOCKCIPHER,
	BC_INIT_TYPE_2,
	present80_str,
	sizeof(present80_ctx_t),
	64,
	{(void_fpt)present80_init},
	{(void_fpt)present80_enc},
	{(void_fpt)present80_dec},
	(bc_free_fpt)NULL,
	present80_keysize_desc
};


