/* mgf1.h */
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

#ifndef MGF1_H_
#define MGF1_H_

#include <stdint.h>
#include "hfal-basic.h"

typedef struct {
	const hfdesc_t* hashfunction;
} mgf1_parameter_t;

/*
 * MGF1 as defined in PKCS #1 v2.1 B.2.1
 */

void mgf1(void* dest, const void* seed, uint16_t seed_len_B, uint16_t out_length_B, const mgf1_parameter_t* p);

#endif /* MGF1_H_ */
