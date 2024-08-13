/* present_common.h */
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
#ifndef PRESENT_COMMON_H_
#define PRESENT_COMMON_H_

#include <stdint.h>

uint8_t present_sbox(uint8_t b);
uint8_t present_sbox_inv(uint8_t b);
void present_p(uint8_t* o, uint8_t* i);

void present_generic_enc(void* buffer, uint8_t* ctx, uint8_t ksize_B,
	void(*update)(uint8_t*, uint8_t));
void present_generic_dec(void* buffer, uint8_t* ctx, uint8_t ksize_B,
	void(*update)(uint8_t*, uint8_t));

#endif /*PRESENT_H_*/
