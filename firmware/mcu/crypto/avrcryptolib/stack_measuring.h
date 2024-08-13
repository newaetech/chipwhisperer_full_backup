/* stack_measuring.h */
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

#ifndef STACK_MEASURING_H_
#define STACK_MEASURING_H_

#include <stdint.h>

typedef struct {
	uint16_t heap_top;
	uint16_t stack_bottom;
	uint8_t pattern;
} stack_measuring_ctx_t;

void stack_measure_init(stack_measuring_ctx_t* ctx, uint8_t pattern);
uint16_t stack_measure_final(const stack_measuring_ctx_t* ctx);
/* for testing only
 void stack_measure_usestack(uint16_t size, uint8_t value); */

#endif /* STACK_MEASURING_H_ */
