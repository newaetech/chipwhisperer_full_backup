/* hfal_jh.c */
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
 * \file     hfal_jh.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte
 * \date     2010-12-16
 * \license  GPLv3 or later
 *
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "jh_simple.h"


static const char jh224_str[]   PROGMEM = "JH-224";
static const char jh256_str[]   PROGMEM = "JH-256";
static const char jh384_str[]   PROGMEM = "JH-384";
static const char jh512_str[]   PROGMEM = "JH-512";

const hfdesc_t jh224_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	jh224_str,
	sizeof(jh_ctx_t),
	JH224_BLOCKSIZE,
	224,
	(hf_init_fpt)jh224_init,
	(hf_nextBlock_fpt)jh_nextBlock,
	(hf_lastBlock_fpt)jh_lastBlock,
	(hf_ctx2hash_fpt)jh224_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};

const hfdesc_t jh256_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	jh256_str,
	sizeof(jh_ctx_t),
	JH256_BLOCKSIZE,
	256,
	(hf_init_fpt)jh256_init,
	(hf_nextBlock_fpt)jh_nextBlock,
	(hf_lastBlock_fpt)jh_lastBlock,
	(hf_ctx2hash_fpt)jh256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};

const hfdesc_t jh384_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	jh384_str,
	sizeof(jh_ctx_t),
	JH384_BLOCKSIZE,
	384,
	(hf_init_fpt)jh384_init,
	(hf_nextBlock_fpt)jh_nextBlock,
	(hf_lastBlock_fpt)jh_lastBlock,
	(hf_ctx2hash_fpt)jh384_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};

const hfdesc_t jh512_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	jh512_str,
	sizeof(jh_ctx_t),
	JH512_BLOCKSIZE,
	512,
	(hf_init_fpt)jh512_init,
	(hf_nextBlock_fpt)jh_nextBlock,
	(hf_lastBlock_fpt)jh_lastBlock,
	(hf_ctx2hash_fpt)jh512_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};


