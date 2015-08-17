/* hfal_groestl_small.c */
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
 * \file     hfal_groestl_small.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-05-05
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "groestl_small.h"


static const char groestl224_str[]   PROGMEM = "Groestl-224";
static const char groestl256_str[]   PROGMEM = "Groestl-256";

const hfdesc_t groestl224_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	groestl224_str,
	sizeof(groestl224_ctx_t),
	GROESTL224_BLOCKSIZE,
	224,
	(hf_init_fpt)groestl224_init,
	(hf_nextBlock_fpt)groestl_small_nextBlock,
	(hf_lastBlock_fpt)groestl_small_lastBlock,
	(hf_ctx2hash_fpt)groestl224_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)groestl224
};

const hfdesc_t groestl256_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	groestl256_str,
	sizeof(groestl256_ctx_t),
	GROESTL256_BLOCKSIZE,
	256,
	(hf_init_fpt)groestl256_init,
	(hf_nextBlock_fpt)groestl_small_nextBlock,
	(hf_lastBlock_fpt)groestl_small_lastBlock,
	(hf_ctx2hash_fpt)groestl256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)groestl256
};


