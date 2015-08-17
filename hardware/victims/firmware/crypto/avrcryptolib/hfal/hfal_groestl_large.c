/* hfal_groestl_large.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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
 * \file     hfal_groestl_large.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-05-05
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "groestl_large.h"
#include "groestl_small.h"


static const char groestl384_str[]   PROGMEM = "Groestl-384";
static const char groestl512_str[]   PROGMEM = "Groestl-512";

const hfdesc_t groestl384_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	groestl384_str,
	sizeof(groestl384_ctx_t),
	GROESTL384_BLOCKSIZE,
	384,
	(hf_init_fpt)groestl384_init,
	(hf_nextBlock_fpt)groestl_large_nextBlock,
	(hf_lastBlock_fpt)groestl_large_lastBlock,
	(hf_ctx2hash_fpt)groestl384_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)groestl384
};

const hfdesc_t groestl512_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	groestl512_str,
	sizeof(groestl512_ctx_t),
	GROESTL512_BLOCKSIZE,
	512,
	(hf_init_fpt)groestl512_init,
	(hf_nextBlock_fpt)groestl_large_nextBlock,
	(hf_lastBlock_fpt)groestl_large_lastBlock,
	(hf_ctx2hash_fpt)groestl512_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)groestl512
};

