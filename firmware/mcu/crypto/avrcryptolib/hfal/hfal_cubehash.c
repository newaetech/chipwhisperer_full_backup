/* hfal_cubehash.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)

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
 * \file     hfal_cubehash.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte
 * \date     2010-02-09
 * \license  GPLv3 or later
 *
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "cubehash.h"


static const char cubehash224_str[]   PROGMEM = "CubeHash-224";
static const char cubehash256_str[]   PROGMEM = "CubeHash-256";
static const char cubehash384_str[]   PROGMEM = "CubeHash-384";
static const char cubehash512_str[]   PROGMEM = "CubeHash-512";

const hfdesc_t cubehash224_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	cubehash224_str,
	sizeof(cubehash_ctx_t),
	CUBEHASH224_BLOCKSIZE,
	224,
	(hf_init_fpt)cubehash224_init,
	(hf_nextBlock_fpt)cubehash_nextBlock,
	(hf_lastBlock_fpt)cubehash_lastBlock,
	(hf_ctx2hash_fpt)cubehash224_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};

const hfdesc_t cubehash256_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	cubehash256_str,
	sizeof(cubehash_ctx_t),
	CUBEHASH256_BLOCKSIZE,
	256,
	(hf_init_fpt)cubehash256_init,
	(hf_nextBlock_fpt)cubehash_nextBlock,
	(hf_lastBlock_fpt)cubehash_lastBlock,
	(hf_ctx2hash_fpt)cubehash256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};

const hfdesc_t cubehash384_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	cubehash384_str,
	sizeof(cubehash_ctx_t),
	CUBEHASH384_BLOCKSIZE,
	384,
	(hf_init_fpt)cubehash384_init,
	(hf_nextBlock_fpt)cubehash_nextBlock,
	(hf_lastBlock_fpt)cubehash_lastBlock,
	(hf_ctx2hash_fpt)cubehash384_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};

const hfdesc_t cubehash512_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	cubehash512_str,
	sizeof(cubehash_ctx_t),
	CUBEHASH512_BLOCKSIZE,
	512,
	(hf_init_fpt)cubehash512_init,
	(hf_nextBlock_fpt)cubehash_nextBlock,
	(hf_lastBlock_fpt)cubehash_lastBlock,
	(hf_ctx2hash_fpt)cubehash512_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};


