/* hfal_blake_large.c */
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
 * \file     hfal_blake_large.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-05-08
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "blake_large.h"


static const char blake384_str[]   PROGMEM = "Blake-384";
static const char blake512_str[]   PROGMEM = "Blake-512";

const hfdesc_t blake384_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	blake384_str,
	sizeof(blake384_ctx_t),
	BLAKE384_BLOCKSIZE,
	384,
	(hf_init_fpt)blake384_init,
	(hf_nextBlock_fpt)blake_large_nextBlock,
	(hf_lastBlock_fpt)blake_large_lastBlock,
	(hf_ctx2hash_fpt)blake384_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)blake384
};

const hfdesc_t blake512_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	blake512_str,
	sizeof(blake512_ctx_t),
	BLAKE512_BLOCKSIZE,
	512,
	(hf_init_fpt)blake512_init,
	(hf_nextBlock_fpt)blake_large_nextBlock,
	(hf_lastBlock_fpt)blake_large_lastBlock,
	(hf_ctx2hash_fpt)blake512_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)blake512
};


