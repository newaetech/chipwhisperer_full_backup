/* hfal_blake_small.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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
 * \file     hfal_blake_small.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-05-05
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "blake_small.h"


static const char blake224_str[]   PROGMEM = "Blake-224";
static const char blake256_str[]   PROGMEM = "Blake-256";

const hfdesc_t blake224_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	blake224_str,
	sizeof(blake224_ctx_t),
	BLAKE224_BLOCKSIZE,
	224,
	(hf_init_fpt)blake224_init,
	(hf_nextBlock_fpt)blake_small_nextBlock,
	(hf_lastBlock_fpt)blake_small_lastBlock,
	(hf_ctx2hash_fpt)blake224_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)blake224
};

const hfdesc_t blake256_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	blake256_str,
	sizeof(blake256_ctx_t),
	BLAKE256_BLOCKSIZE,
	256,
	(hf_init_fpt)blake256_init,
	(hf_nextBlock_fpt)blake_small_nextBlock,
	(hf_lastBlock_fpt)blake_small_lastBlock,
	(hf_ctx2hash_fpt)blake256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)blake256
};


