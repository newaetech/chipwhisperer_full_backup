/* hfal_shabal.c */
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
 * \file     hfal_shabal.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-04-20
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "shabal.h"


static const char shabal192_str[]   PROGMEM = "Shabal-192";
static const char shabal224_str[]   PROGMEM = "Shabal-224";
static const char shabal256_str[]   PROGMEM = "Shabal-256";
static const char shabal384_str[]   PROGMEM = "Shabal-384";
static const char shabal512_str[]   PROGMEM = "Shabal-512";

const hfdesc_t shabal192_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	shabal192_str,
	sizeof(shabal_ctx_t),
	SHABAL_BLOCKSIZE,
	192,
	(hf_init_fpt)shabal192_init,
	(hf_nextBlock_fpt)shabal_nextBlock,
	(hf_lastBlock_fpt)shabal_lastBlock,
	(hf_ctx2hash_fpt)shabal192_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)shabal192
};

const hfdesc_t shabal224_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	shabal224_str,
	sizeof(shabal_ctx_t),
	SHABAL_BLOCKSIZE,
	224,
	(hf_init_fpt)shabal224_init,
	(hf_nextBlock_fpt)shabal_nextBlock,
	(hf_lastBlock_fpt)shabal_lastBlock,
	(hf_ctx2hash_fpt)shabal224_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)shabal224
};

const hfdesc_t shabal256_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	shabal256_str,
	sizeof(shabal_ctx_t),
	SHABAL_BLOCKSIZE,
	256,
	(hf_init_fpt)shabal256_init,
	(hf_nextBlock_fpt)shabal_nextBlock,
	(hf_lastBlock_fpt)shabal_lastBlock,
	(hf_ctx2hash_fpt)shabal256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)shabal256
};

const hfdesc_t shabal384_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	shabal384_str,
	sizeof(shabal_ctx_t),
	SHABAL_BLOCKSIZE,
	384,
	(hf_init_fpt)shabal384_init,
	(hf_nextBlock_fpt)shabal_nextBlock,
	(hf_lastBlock_fpt)shabal_lastBlock,
	(hf_ctx2hash_fpt)shabal384_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)shabal384
};

const hfdesc_t shabal512_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	shabal512_str,
	sizeof(shabal_ctx_t),
	SHABAL_BLOCKSIZE,
	512,
	(hf_init_fpt)shabal512_init,
	(hf_nextBlock_fpt)shabal_nextBlock,
	(hf_lastBlock_fpt)shabal_lastBlock,
	(hf_ctx2hash_fpt)shabal512_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)shabal512
};
