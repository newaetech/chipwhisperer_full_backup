/* gf256mul.c */
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
 * \file     gf256mul.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-01-13
 * \license  GPLv3 or later
 * 
 */
#include <stdint.h>
#include "gf256mul.h"

uint8_t gf256mul(uint8_t a, uint8_t b, uint8_t reducer){
	uint8_t i;
	uint8_t p=0,t;
	for(i=0; i<8; ++i){
		if(b&1)
			p ^= a;
		t=a&0x80;
		a<<=1;
		if(t)
			a ^= reducer;
		b>>=1;
	}
	return p;
}
