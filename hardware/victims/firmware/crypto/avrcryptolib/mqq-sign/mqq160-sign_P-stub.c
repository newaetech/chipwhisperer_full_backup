/* mqq160-sign.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Danilo Gligoroski, Daniel Otte (daniel.otte@rub.de)

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
   C code for MQQ160-SIGN suitable for 8-bit smart cards

   It is supposed that the private key is "engraved" in
   the ROM of the smart card - thus it is here stored as
   predefined const arrays in "MQQ160-SIGN-PrivateKey.h"

   Programmed by Danilo Gligoroski, 18 Mar 2010.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include "memxor.h"
#include "mqq160-sign.h"

/*
This is just for testing purposes.
It should be programmed in a more flexible way
in the MQQ160-SIGN C Library.
*/


void mqq_inv_affine_transformation(uint8_t* input_bytes, uint8_t* result, const mqq160_sign_key_t* key);
uint8_t mqq_q(uint8_t i, uint8_t b1, uint8_t b2, const mqq160_sign_key_t* key);


#if 0
static uint16_t MaskShort[8] = {0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200, 0x0100};

static uint8_t mqq_q(uint8_t i, uint8_t b1, uint8_t b2, const mqq160_sign_key_t* key){
	uint8_t  e[9];
	uint16_t a[8];
	uint8_t result, column, row, k;
	int8_t j;
	uint16_t temp;
	uint8_t *tmp_ptr=key->a;
	if(i&1){
		memcpy_P(e, key->cc1, 9);
		while(b1){
			if(b1&0x80){
				memxor_idx_P((uint8_t*)e, tmp_ptr, 9, 9);
			}
			tmp_ptr++;
			b1 <<= 1;
		}
	}else{
		memcpy_P(e, key->cc2, 9);
		while(b1){
			if(b1&0x80){
				memxor_P((uint8_t*)e, tmp_ptr, 9);
			}
			tmp_ptr+=9;
			b1 <<= 1;
		}
	}
	/* So we finished with obtaining e0 .. e7 and e8 */

	/* We XOR e[8] with b2 and that will be initial value to transform in order to solve a linear system of equations */
	result=b2 ^ e[8];

	/*
	   We can look at the bits of e0 .. e7 as a columns of a given matrix. We want to define 8 variables that have the rows
	   of that matrix. The variables need to be 16-bit because we will put into the upper 8 bits the bits of e0 .. e7,
	   and the bits of the variable result will be the Least Significant Bits of a[0] ... a[7].
   */
	for(j=0; j<8; ++j){
		row = 0;
		for(k=0; k<8; ++k){
			row |= (e[k]&0x80)>>(k);
			e[k]<<=1;
		}
		a[j]=(((uint16_t)row)<<8) | (result>>7);
		result <<= 1;
	}

	/* Now we finally realize Gausian elimination */

	/* First we apply upper triangular transformation */
	for(column=0; column<8; column++)
	{
		row=column;
		while ((a[row] & MaskShort[column]) == 0){
			row++;
		}
		if(row>column)
		{
			temp=a[column];
			a[column]=a[row];
			a[row]=temp;
		}
		for (j=column+1; j<8; j++)
			if ((a[j]&MaskShort[column]) !=0)
				a[j] ^= a[column];
	}

	/* Then we eliminate 1s above the main diagonal */
	for (column=7; column>0; column--){
		for (j=column-1; j>=0; j--){
			if ((a[j]&MaskShort[column]) !=0){
				a[j] ^= a[column];
			}
		}
	}
	/* The result is in the Least Significant Bits of a[0] ... a[7] */
	result = 0;
	for(j=0; j<8; ++j){
		result <<=1;
		result |= a[j]&1;
	}
	return(result);
}

#endif

void mqq160_sign_P(void* dest, const void* hash, const mqq160_sign_key_t* key_P){
	uint8_t i, r1[20], byteindex;
	mqq160_sign_key_t key;

	memcpy_P(&key, key_P, sizeof(mqq160_sign_key_t));

	mqq_inv_affine_transformation((uint8_t*)hash, (uint8_t*)dest, &key);
	r1[0]=((uint8_t*)dest)[0];
	for(i=1; i<20; ++i){
		r1[i] = mqq_q(i, r1[i-1], ((uint8_t*)dest)[i], &key);
	}
	/*
	Affine transformation is just for the second call. The constant is extracted
	from the 4 LSBs of the first 40 bytes of RP5[] and xor-ed to input_bytes[].
	*/
	byteindex = 0;
	for (i=0; i<20; i++){
		r1[i] ^=   (uint8_t)(pgm_read_byte(key.rp5+byteindex)<<4)
				 | (uint8_t)(pgm_read_byte(key.rp5+byteindex+1)&0x0F);
		byteindex += 2;
	}
	mqq_inv_affine_transformation(r1, (uint8_t*)dest, &key);
}
