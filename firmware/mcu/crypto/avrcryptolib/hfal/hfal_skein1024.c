/* hfal_skein1024.c */
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
 * \file     hfal_skein1024.c
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


static const char skein1024_128_str[]   PROGMEM = "Skein-1024-128";
static const char skein1024_160_str[]   PROGMEM = "Skein-1024-160";
static const char skein1024_224_str[]   PROGMEM = "Skein-1024-224";
static const char skein1024_256_str[]   PROGMEM = "Skein-1024-256";
static const char skein1024_384_str[]   PROGMEM = "Skein-1024-384";
static const char skein1024_512_str[]   PROGMEM = "Skein-1024-512";
static const char skein1024_1024_str[]  PROGMEM = "Skein-1024-1024";

void skein1024_128_init(skein1024_ctx_t* ctx){
	skein1024_init(ctx, 128);
}
void skein1024_160_init(skein1024_ctx_t* ctx){
	skein1024_init(ctx, 160);
}
void skein1024_224_init(skein1024_ctx_t* ctx){
	skein1024_init(ctx, 224);
}
void skein1024_256_init(skein1024_ctx_t* ctx){
	skein1024_init(ctx, 256);
}
void skein1024_384_init(skein1024_ctx_t* ctx){
	skein1024_init(ctx, 384);
}
void skein1024_512_init(skein1024_ctx_t* ctx){
	skein1024_init(ctx, 512);
}
void skein1024_1024_init(skein1024_ctx_t* ctx){
	skein1024_init(ctx, 1024);
}

const hfdesc_t skein1024_128_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	skein1024_128_str,
	sizeof(skein1024_ctx_t),
	SKEIN1024_BLOCKSIZE,
	128,
	(hf_init_fpt)skein1024_128_init,
	(hf_nextBlock_fpt)skein1024_nextBlock,
	(hf_lastBlock_fpt)skein1024_lastBlock,
	(hf_ctx2hash_fpt)skein1024_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};
const hfdesc_t skein1024_160_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	skein1024_160_str,
	sizeof(skein1024_ctx_t),
	SKEIN1024_BLOCKSIZE,
	160,
	(hf_init_fpt)skein1024_160_init,
	(hf_nextBlock_fpt)skein1024_nextBlock,
	(hf_lastBlock_fpt)skein1024_lastBlock,
	(hf_ctx2hash_fpt)skein1024_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};
const hfdesc_t skein1024_224_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	skein1024_224_str,
	sizeof(skein1024_ctx_t),
	SKEIN1024_BLOCKSIZE,
	224,
	(hf_init_fpt)skein1024_224_init,
	(hf_nextBlock_fpt)skein1024_nextBlock,
	(hf_lastBlock_fpt)skein1024_lastBlock,
	(hf_ctx2hash_fpt)skein1024_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};
const hfdesc_t skein1024_256_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	skein1024_256_str,
	sizeof(skein1024_ctx_t),
	SKEIN1024_BLOCKSIZE,
	256,
	(hf_init_fpt)skein1024_256_init,
	(hf_nextBlock_fpt)skein1024_nextBlock,
	(hf_lastBlock_fpt)skein1024_lastBlock,
	(hf_ctx2hash_fpt)skein1024_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};
const hfdesc_t skein1024_384_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	skein1024_384_str,
	sizeof(skein1024_ctx_t),
	SKEIN1024_BLOCKSIZE,
	384,
	(hf_init_fpt)skein1024_384_init,
	(hf_nextBlock_fpt)skein1024_nextBlock,
	(hf_lastBlock_fpt)skein1024_lastBlock,
	(hf_ctx2hash_fpt)skein1024_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};
const hfdesc_t skein1024_512_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	skein1024_512_str,
	sizeof(skein1024_ctx_t),
	SKEIN1024_BLOCKSIZE,
	512,
	(hf_init_fpt)skein1024_512_init,
	(hf_nextBlock_fpt)skein1024_nextBlock,
	(hf_lastBlock_fpt)skein1024_lastBlock,
	(hf_ctx2hash_fpt)skein1024_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};
const hfdesc_t skein1024_1024_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	skein1024_1024_str,
	sizeof(skein1024_ctx_t),
	SKEIN1024_BLOCKSIZE,
	1024,
	(hf_init_fpt)skein1024_1024_init,
	(hf_nextBlock_fpt)skein1024_nextBlock,
	(hf_lastBlock_fpt)skein1024_lastBlock,
	(hf_ctx2hash_fpt)skein1024_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};

