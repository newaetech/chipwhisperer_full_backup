/* hfal_twister256.c */
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
 * \file     hfal_twister256.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-02-09
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "twister-small.h"

static const char twister256_str[]   PROGMEM = "Twister-256";

const hfdesc_t twister256_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	twister256_str,
	sizeof(twister256_ctx_t),
	512,
	256,
	(hf_init_fpt)twister256_init,
	(hf_nextBlock_fpt)twister256_nextBlock,
	(hf_lastBlock_fpt)twister256_lastBlock,
	(hf_ctx2hash_fpt)twister256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)twister256
};

