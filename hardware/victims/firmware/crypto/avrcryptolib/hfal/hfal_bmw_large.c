/* hfal_bmw_large.c */
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
 * \file     hfal_bmw_large.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-04-28
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "bmw_large.h"


static const char bmw384_str[]   PROGMEM = "BlueMidnightWish-384";
static const char bmw512_str[]   PROGMEM = "BlueMidnightWish-512";

const hfdesc_t bmw384_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	bmw384_str,
	sizeof(bmw384_ctx_t),
	BMW384_BLOCKSIZE,
	384,
	(hf_init_fpt)bmw384_init,
	(hf_nextBlock_fpt)bmw384_nextBlock,
	(hf_lastBlock_fpt)bmw384_lastBlock,
	(hf_ctx2hash_fpt)bmw384_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)bmw384
};

const hfdesc_t bmw512_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	bmw512_str,
	sizeof(bmw512_ctx_t),
	BMW512_BLOCKSIZE,
	512,
	(hf_init_fpt)bmw512_init,
	(hf_nextBlock_fpt)bmw512_nextBlock,
	(hf_lastBlock_fpt)bmw512_lastBlock,
	(hf_ctx2hash_fpt)bmw512_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)bmw512
};


