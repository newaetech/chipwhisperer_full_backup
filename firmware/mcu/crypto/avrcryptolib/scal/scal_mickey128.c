/* scal_mickey128.c */
/*
    This file is part of the AVR-Crypto-Lib.
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

#include <stdlib.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include "streamcipher_descriptor.h"
#include "keysize_descriptor.h"

#include "mickey128.h"

const char mickey128_str[]   PROGMEM = "Mickey128";

const uint8_t mickey128_keysize_desc[] PROGMEM = {
		KS_TYPE_LIST, 1, KS_INT(128),
		KS_TYPE_TERMINATOR   };

const uint8_t mickey128_ivsize_desc[] PROGMEM = {
		KS_TYPE_RANGE, KS_INT(0), KS_INT(128),
        KS_TYPE_TERMINATOR   };

const scdesc_t mickey128_desc PROGMEM = {
		SCDESC_TYPE_STREAMCIPHER,      /* abstraction layer type designator */
		SC_INIT_TYPE_5|SC_GEN_TYPE_1,  /* flags*/
		mickey128_str,                 /* name string pointer */
		sizeof(mickey128_ctx_t),       /* size of context */
		8,                             /* blocksize */
		{(void_fpt)mickey128_init},    /* init function pointer */
		{(void_fpt)mickey128_getbyte}, /* key stream generator function pointer */
		{(void_fpt)NULL},              /* key stream generator for random access function pointer */
		(sc_free_fpt)NULL,	           /* free function pointer */
		mickey128_keysize_desc,        /* key size descriptor pointer */
		mickey128_ivsize_desc          /* iv size descriptor pointer */
};






