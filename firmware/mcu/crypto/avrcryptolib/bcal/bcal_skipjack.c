/* bcal_skipjack.c */
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
 * \file     bcal_skipjack.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-01-09
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "blockcipher_descriptor.h"
#include "skipjack.h"
#include "keysize_descriptor.h"

const char skipjack_str[]   PROGMEM = "Skipjack";

const uint8_t skipjack_keysize_desc[] PROGMEM = { KS_TYPE_LIST, 1, KS_INT(80), 
                                                  KS_TYPE_TERMINATOR    };

const bcdesc_t skipjack_desc PROGMEM = {
	BCDESC_TYPE_BLOCKCIPHER,
	BC_INIT_TYPE_1,
	skipjack_str,
	10,
	64,
	{(void_fpt)NULL},
	{(void_fpt)skipjack_enc},
	{(void_fpt)skipjack_dec},
	(bc_free_fpt)NULL,
	skipjack_keysize_desc
};


