/* bcal_khazad.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2011  Daniel Otte (daniel.otte@rub.de)

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
 * \file     bcal_khazad.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2011-01-02
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "blockcipher_descriptor.h"
#include "khazad.h"
#include "keysize_descriptor.h"

const char khazad_str[]   PROGMEM = "Khazad";

const uint8_t khazad_keysize_desc[] PROGMEM = { KS_TYPE_LIST, 1, KS_INT(128),
                                                KS_TYPE_TERMINATOR    };


const bcdesc_t khazad_desc PROGMEM = {
	BCDESC_TYPE_BLOCKCIPHER,
	BC_INIT_TYPE_1,
	khazad_str,
	sizeof(khazad_ctx_t),
	64,
	{(void_fpt)khazad_init},
	{(void_fpt)khazad_enc},
	{(void_fpt)khazad_dec},
	(bc_free_fpt)NULL,
	khazad_keysize_desc
};


