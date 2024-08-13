/* rotate64.h */
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

#ifndef ROTATE64_H_
#define ROTATE64_H_

#include <stdint.h>

#define ROTL_0 0
#define ROTL_1 1
#define ROTL_2 2
#define ROTL_3 3
#define ROTL_4 4
#define ROTR_0 0
#define ROTR_1 (8+1)
#define ROTR_2 (8+2)
#define ROTR_3 (8+3)


#define ROT_BIT(a) (((a)<=4)?((a)<<1):(0x01|((8-(a))<<1)))

//#define ROT_CODE(a,b) (((a)<<4) | ((b)&0x0f))

#define ROT_CODE(a) ((((a)/8+((((a)%8)>4)?1:0))<<4) | ROT_BIT(((a)%8)))
//#define ROT_CODE(a) ((((a)/8)<<4) | ((a)%8)) /* rotations only to the left */

uint64_t rotate64_1bit_left(uint64_t a);
uint64_t rotate64_1bit_right(uint64_t a);
uint64_t rotate64_nbit_autodir(uint64_t a, int8_t n);
uint64_t rotate64left_code(uint64_t a, int8_t code);

#endif /* ROTATE64_H_ */
