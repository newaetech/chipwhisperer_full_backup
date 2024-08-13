/* hfal_echo.c */
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
 * \file     hfal_echo.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte
 * \date     2010-02-21
 * \license  GPLv3 or later
 *
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "echo.h"


static const char echo224_str[]   PROGMEM = "ECHO-224";
static const char echo256_str[]   PROGMEM = "ECHO-256";
static const char echo384_str[]   PROGMEM = "ECHO-384";
static const char echo512_str[]   PROGMEM = "ECHO-512";

const hfdesc_t echo224_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	echo224_str,
	sizeof(echo_small_ctx_t),
	ECHO224_BLOCKSIZE,
	224,
	(hf_init_fpt)echo224_init,
	(hf_nextBlock_fpt)echo_small_nextBlock,
	(hf_lastBlock_fpt)echo_small_lastBlock,
	(hf_ctx2hash_fpt)echo224_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};

const hfdesc_t echo256_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	echo256_str,
	sizeof(echo_small_ctx_t),
	ECHO256_BLOCKSIZE,
	256,
	(hf_init_fpt)echo256_init,
	(hf_nextBlock_fpt)echo_small_nextBlock,
	(hf_lastBlock_fpt)echo_small_lastBlock,
	(hf_ctx2hash_fpt)echo256_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};

const hfdesc_t echo384_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	echo384_str,
	sizeof(echo_large_ctx_t),
	ECHO384_BLOCKSIZE,
	384,
	(hf_init_fpt)echo384_init,
	(hf_nextBlock_fpt)echo_large_nextBlock,
	(hf_lastBlock_fpt)echo_large_lastBlock,
	(hf_ctx2hash_fpt)echo384_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};

const hfdesc_t echo512_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	echo512_str,
	sizeof(echo_large_ctx_t),
	ECHO512_BLOCKSIZE,
	512,
	(hf_init_fpt)echo512_init,
	(hf_nextBlock_fpt)echo_large_nextBlock,
	(hf_lastBlock_fpt)echo_large_lastBlock,
	(hf_ctx2hash_fpt)echo512_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)NULL
};


