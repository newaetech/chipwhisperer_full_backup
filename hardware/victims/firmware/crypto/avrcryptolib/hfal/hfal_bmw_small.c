/* hfal_bmw_small.c */
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
 * \file     hfal_bmw_small.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-04-28
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "bmw_small.h"


static const char bmw224_str[]   PROGMEM = "BlueMidnightWish-224";
static const char bmw256_str[]   PROGMEM = "BlueMidnightWish-256";

const hfdesc_t bmw224_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	bmw224_str,
	sizeof(bmw224_ctx_t),
	BMW224_BLOCKSIZE,
	224,
	(hf_init_fpt)bmw224_init,
	(hf_nextBlock_fpt)bmw224_nextBlock,
	(hf_lastBlock_fpt)bmw224_lastBlock,
	(hf_ctx2hash_fpt)bmw224_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)bmw224
};

const hfdesc_t bmw256_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	bmw256_str,
	sizeof(bmw256_ctx_t),
	BMW256_BLOCKSIZE,
	256,
	(hf_init_fpt)bmw256_init,
	(hf_nextBlock_fpt)bmw256_nextBlock,
	(hf_lastBlock_fpt)bmw256_lastBlock,
	(hf_ctx2hash_fpt)bmw256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)bmw256
};


