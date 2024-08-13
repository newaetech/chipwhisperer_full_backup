/* keysize_descriptor.h */
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
 * \file    keysize_descriptor.h 
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-01-07
 * \license GPLv3 or later
 */

#ifndef KEYSIZE_DESCRIPTOR_H_
#define KEYSIZE_DESCRIPTOR_H_

#include <stdint.h>
#include <avr/pgmspace.h>

#define KS_TYPE_TERMINATOR 0x00
#define KS_TYPE_LIST       0x01
#define KS_TYPE_RANGE      0x02
#define KS_TYPE_ARG_RANGE  0x03

#define KS_INT(a) ((a)&0xFF), ((a)>>8)

typedef struct{ /* keysize is valid if listed in items */
	uint8_t  n_items;  /* number of items (value 0 is reserved) */
	uint16_t items[]; /* list of valid lengths */
}keysize_desc_list_t;

typedef struct{ /* keysize is valid if min<=keysize<=max */
	uint16_t min;
	uint16_t max;
}keysize_desc_range_t;

typedef struct{ /* keysize is valid if min<=keysize<=max and if keysize mod distance == offset */
	uint16_t min;
	uint16_t max;
	uint16_t distance;
	uint16_t offset;
}keysize_desc_arg_range_t;

uint8_t is_valid_keysize_P(PGM_VOID_P ks_desc, uint16_t keysize);
uint16_t get_keysize(PGM_VOID_P ks_desc);
uint16_t get_keysizes(PGM_VOID_P ks_desc, uint16_t** list);


#endif /* KEYSIZE_DESCRIPTOR_H_ */
