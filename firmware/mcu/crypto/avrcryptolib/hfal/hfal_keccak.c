/* hfal_keccak.c */
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
 * \file     hfal_keccak.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2010-02-09
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "keccak.h"


static const char keccak224_str[]   PROGMEM = "Keccak-224";
static const char keccak256_str[]   PROGMEM = "Keccak-256";
static const char keccak384_str[]   PROGMEM = "Keccak-384";
static const char keccak512_str[]   PROGMEM = "Keccak-512";

const hfdesc_t keccak224_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	keccak224_str,
	sizeof(keccak_ctx_t),
	KECCAK224_BLOCKSIZE,
	224,
	(hf_init_fpt)keccak224_init,
	(hf_nextBlock_fpt)keccak_nextBlock,
	(hf_lastBlock_fpt)keccak_lastBlock,
	(hf_ctx2hash_fpt)keccak224_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};

const hfdesc_t keccak256_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	keccak256_str,
	sizeof(keccak_ctx_t),
	KECCAK256_BLOCKSIZE,
	256,
	(hf_init_fpt)keccak256_init,
	(hf_nextBlock_fpt)keccak_nextBlock,
	(hf_lastBlock_fpt)keccak_lastBlock,
	(hf_ctx2hash_fpt)keccak256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};

const hfdesc_t keccak384_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	keccak384_str,
	sizeof(keccak_ctx_t),
	KECCAK384_BLOCKSIZE,
	384,
	(hf_init_fpt)keccak384_init,
	(hf_nextBlock_fpt)keccak_nextBlock,
	(hf_lastBlock_fpt)keccak_lastBlock,
	(hf_ctx2hash_fpt)keccak384_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};

const hfdesc_t keccak512_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	keccak512_str,
	sizeof(keccak_ctx_t),
	KECCAK512_BLOCKSIZE,
	512,
	(hf_init_fpt)keccak512_init,
	(hf_nextBlock_fpt)keccak_nextBlock,
	(hf_lastBlock_fpt)keccak_lastBlock,
	(hf_ctx2hash_fpt)keccak512_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};


