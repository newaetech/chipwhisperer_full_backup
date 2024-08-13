/* hfal_sha224.c */
/*
    This file is part of the ARM-Crypto-Lib.
    Copyright (C) 2011  Daniel Otte (daniel.otte@rub.de)

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
 * \file     hfal_sha224.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2011-10-11
 * \license  GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "sha224.h"

static const char sha224_str[] PROGMEM = "SHA-224";

const hfdesc_t sha224_desc PROGMEM = {
	HFDESC_TYPE_HASHFUNCTION,
	0,
	sha224_str,
	sizeof(sha224_ctx_t),
	512,
	224,
	(hf_init_fpt)sha224_init,
	(hf_nextBlock_fpt)sha224_nextBlock,
	(hf_lastBlock_fpt)sha224_lastBlock,
	(hf_ctx2hash_fpt)sha224_ctx2hash,
	(hf_free_fpt)NULL,
	(hf_mem_fpt)sha224
};

