/* mgf1.c */
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

#include <stdint.h>
#include <string.h>
#include "mgf1.h"
#include "hfal-basic.h"

/*
 * MGF1 as defined in PKCS #1 v2.1 B.2.1
 */


void mgf1_short_seed(void* dest, const void* seed, uint8_t seed_len_B, uint16_t out_length_B, const mgf1_parameter_t* p){
	uint8_t buffer[seed_len_B+4];
	uint32_t counter=1;
	uint8_t hv_len = hfal_hash_getHashsize(p->hashfunction)/8;
	memcpy(buffer, seed, seed_len_B);
	memset(buffer + seed_len_B, 0, 4);
	while(out_length_B >= hv_len){
		hfal_hash_mem(p->hashfunction, dest, buffer, (seed_len_B + 4) * 8);
		dest = (uint8_t*)dest + hv_len;
		out_length_B -= hv_len;
		buffer[seed_len_B + 3] = counter & 0xff;
		buffer[seed_len_B + 2] = (counter>>8) & 0xff;
		buffer[seed_len_B + 1] = (counter>>16) & 0xff;
		buffer[seed_len_B + 0] = (counter>>24) & 0xff;
		++counter;
	}
	if(out_length_B){
		uint8_t hash_buffer[hv_len];
		hfal_hash_mem(p->hashfunction, hash_buffer, buffer, (seed_len_B + 4) * 8);
		memcpy(dest, hash_buffer, out_length_B);
	}
}

void mgf1(void* dest, const void* seed, uint16_t seed_len_B, uint16_t out_length_B, const mgf1_parameter_t* p){
	mgf1_short_seed(dest, seed, seed_len_B, out_length_B, p);
}
