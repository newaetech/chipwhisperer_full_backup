/* scal_rabbit.c */
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

#include "rabbit.h"

const char rabbit_str[]   PROGMEM = "Rabbit";

const uint8_t rabbit_keysize_desc[] PROGMEM = {
		KS_TYPE_LIST, 2, KS_INT(128), KS_INT(80),
        KS_TYPE_TERMINATOR   };

const uint8_t rabbit_ivsize_desc[] PROGMEM = {
		KS_TYPE_LIST, 1, KS_INT(64),
        KS_TYPE_TERMINATOR   };

const scdesc_t rabbit_desc PROGMEM = {
		SCDESC_TYPE_STREAMCIPHER,     /* abstraction layer type designator */
		SC_INIT_TYPE_4|SC_GEN_TYPE_1, /* flags*/
		rabbit_str,                  /* name string pointer */
		sizeof(rabbit_ctx_t),        /* size of context */
		8,                            /* blocksize */
		{(void_fpt)rabbit_init},     /* init function pointer */
		{(void_fpt)rabbit_gen},      /* key stream generator function pointer */
		{(void_fpt)NULL},             /* key stream generator for random access function pointer */
		(sc_free_fpt)NULL,	          /* free function pointer */
		rabbit_keysize_desc,         /* key size descriptor pointer */
		rabbit_ivsize_desc           /* iv size descriptor pointer */
};






