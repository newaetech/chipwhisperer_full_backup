/* threefish_invmix_c.c */
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
/*
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-03-21
 * \license GPLv3 or later
 * 
 * 
 * 
 */

#include <stdint.h>

#define X0 (((uint64_t*)data)[0])
#define X1 (((uint64_t*)data)[1])
void threefish_invmix(void* data, uint8_t rot){
	uint64_t x;
	x = X1;
	x ^= X0;
	X1 = ((x>>rot)|(x<<(64-rot)));
	X0 -= X1;
}
