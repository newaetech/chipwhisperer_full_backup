/* hfal_skein256.c */
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
 * \file     hfal_skein256.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-03-13
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "skein.h"


static const char skein256_128_str[]   PROGMEM = "Skein-256-128";
static const char skein256_160_str[]   PROGMEM = "Skein-256-160";
static const char skein256_224_str[]   PROGMEM = "Skein-256-224";
static const char skein256_256_str[]   PROGMEM = "Skein-256-256";
static const char skein256_384_str[]   PROGMEM = "Skein-256-384";
static const char skein256_512_str[]   PROGMEM = "Skein-256-512";

void skein256_128_init(skein256_ctx_t* ctx){
	skein256_init(ctx, 128);
}
void skein256_160_init(skein256_ctx_t* ctx){
	skein256_init(ctx, 160);
}
void skein256_224_init(skein256_ctx_t* ctx){
	skein256_init(ctx, 224);
}
void skein256_256_init(skein256_ctx_t* ctx){
	skein256_init(ctx, 256);
}
void skein256_384_init(skein256_ctx_t* ctx){
	skein256_init(ctx, 384);
}
void skein256_512_init(skein256_ctx_t* ctx){
	skein256_init(ctx, 512);
}

const hfdesc_t skein256_128_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	skein256_128_str,
	sizeof(skein256_ctx_t),
	SKEIN256_BLOCKSIZE,
	128,
	(hf_init_fpt)skein256_128_init,
	(hf_nextBlock_fpt)skein256_nextBlock,
	(hf_lastBlock_fpt)skein256_lastBlock,
	(hf_ctx2hash_fpt)skein256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};
const hfdesc_t skein256_160_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	skein256_160_str,
	sizeof(skein256_ctx_t),
	SKEIN256_BLOCKSIZE,
	160,
	(hf_init_fpt)skein256_160_init,
	(hf_nextBlock_fpt)skein256_nextBlock,
	(hf_lastBlock_fpt)skein256_lastBlock,
	(hf_ctx2hash_fpt)skein256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};
const hfdesc_t skein256_224_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	skein256_224_str,
	sizeof(skein256_ctx_t),
	SKEIN256_BLOCKSIZE,
	224,
	(hf_init_fpt)skein256_224_init,
	(hf_nextBlock_fpt)skein256_nextBlock,
	(hf_lastBlock_fpt)skein256_lastBlock,
	(hf_ctx2hash_fpt)skein256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};
const hfdesc_t skein256_256_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	skein256_256_str,
	sizeof(skein256_ctx_t),
	SKEIN256_BLOCKSIZE,
	256,
	(hf_init_fpt)skein256_256_init,
	(hf_nextBlock_fpt)skein256_nextBlock,
	(hf_lastBlock_fpt)skein256_lastBlock,
	(hf_ctx2hash_fpt)skein256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};
const hfdesc_t skein256_384_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	skein256_384_str,
	sizeof(skein256_ctx_t),
	SKEIN256_BLOCKSIZE,
	384,
	(hf_init_fpt)skein256_384_init,
	(hf_nextBlock_fpt)skein256_nextBlock,
	(hf_lastBlock_fpt)skein256_lastBlock,
	(hf_ctx2hash_fpt)skein256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};
const hfdesc_t skein256_512_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	skein256_512_str,
	sizeof(skein256_ctx_t),
	SKEIN256_BLOCKSIZE,
	512,
	(hf_init_fpt)skein256_512_init,
	(hf_nextBlock_fpt)skein256_nextBlock,
	(hf_lastBlock_fpt)skein256_lastBlock,
	(hf_ctx2hash_fpt)skein256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};
