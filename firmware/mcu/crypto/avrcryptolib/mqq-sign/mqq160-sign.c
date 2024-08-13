/* mqq160-sign.c */
/*
   C code for MQQ160-SIGN suitable for 8-bit smart cards

   It is supposed that the private key is "engraved" in
   the ROM of the smart card - thus it is here stored as
   predefined const arrays in "MQQ160-SIGN-PrivateKey.h"

   Programmed by
   Danilo Gligoroski and Rune Jensen and Daniel Otte
   March 2010.

   Verified by Danilo Gligoroski
   March 2010.

*/

#include <string.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include "memxor.h"
#include "mqq160-sign.h"

#include "cli.h"

static const uint8_t mod20_table[32] PROGMEM = {
		 4,  5,  6,  7,  8,  9, 10, 11,
		12, 13, 14, 15, 16, 17, 18, 19,
		 0,  1,  2,  3,  4,  5,  6,  7,
		 8,  9, 10, 11, 12, 13, 14, 15,
};

static void memxor_idx(void* dest, const void* src, uint16_t length, uint8_t dist){
	while(length--){
		*((uint8_t*)dest) ^= *((uint8_t*)src);
		dest = (uint8_t*)dest + 1;
		src  = (uint8_t*)src  + dist;
	}
}
/*
This is just for testing purposes.
It should be programmed in a more flexible way
in the MQQ160-SIGN C Library.
*/

static void mqq_inv_affine_transformation(const uint8_t* input_bytes, uint8_t* result, const mqq160_sign_key_t* key){
	/* The matrix SInv is given as two permutations of 160 elements. */
	uint8_t j, byteindex, bitindex, bitindex_d, byteindex_d, rp1, rp5;
	const uint8_t *rp1_ptr, *rp5_ptr;
	uint8_t h1[20];


	/* Initialize H1 and H2 = 0 */
	memset(h1, 0, 20);
	memset(result, 0, 20);

	/*
	   Fill H1 with bits of InputBytes accordingly to RP1 permutation
	   and fill H2 with bits of InputBytes accordingly to RP5 permutation
	*/
	j=160;
	byteindex_d = 0;
	bitindex_d = 0x80;
	rp1_ptr = key->rp1;
	rp5_ptr = key->rp5;
	do{
		rp1 = *rp1_ptr++;
		rp5 = *rp5_ptr++;
		byteindex = rp1>>3;
		bitindex = 0x80 >> (rp1&0x07);
		if (input_bytes[byteindex] & bitindex){
			h1[byteindex_d] ^= bitindex_d;
		}

		byteindex = rp5>>3;
		bitindex = 0x80 >> (rp5&0x07);
		if (input_bytes[byteindex] & bitindex){
			result[byteindex_d] ^= bitindex_d;
		}
		bitindex_d >>= 1;
		if(bitindex_d==0){
			++byteindex_d;
			bitindex_d = 0x80;
		}
	}while(--j);
//	cli_putstr_P(PSTR("\r\nDBG (ref): "));
//	cli_hexdump(h1, 20);
	for (j=0; j<20; j++){
		result[j] ^= h1[j] ^ h1[pgm_read_byte(j+mod20_table)]
		                   ^ h1[pgm_read_byte(8+j+mod20_table)]
		                   ^ h1[pgm_read_byte(12+j+mod20_table)];
	}
}

static uint16_t MaskShort[8] = {0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200, 0x0100};

static uint8_t mqq_q(uint8_t i, uint8_t b1, uint8_t b2, const mqq160_sign_key_t* key){
	uint8_t  e[9];
	uint16_t a[8];
	uint8_t result, column, row, k;
	int8_t j;
	uint16_t temp;
	const uint8_t *tmp_ptr=key->a;
	if(i&1){
		memcpy(e, key->cc1, 9);
		while(b1){
			if(b1&0x80){
				memxor_idx((uint8_t*)e, tmp_ptr, 9, 9);
			}
			tmp_ptr++;
			b1 <<= 1;
		}
	}else{
		memcpy(e, key->cc2, 9);
		while(b1){
			if(b1&0x80){
				memxor((uint8_t*)e, tmp_ptr, 9);
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
			if ((a[j]&MaskShort[column]) !=0){
				a[j] ^= a[column];
			}
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

void mqq160_sign(void* dest, const void* hash, const mqq160_sign_key_t* key){
	uint8_t i, r1[20], byteindex;
	mqq_inv_affine_transformation((uint8_t*)hash, (uint8_t*)dest, key);
	r1[0]=((uint8_t*)dest)[0];
	for(i=1; i<20; ++i){
		r1[i] = mqq_q(i, r1[i-1], ((uint8_t*)dest)[i], key);
	}
	/*
	Affine transformation is just for the second call. The constant is extracted
	from the 4 LSBs of the first 40 bytes of RP5[] and xor-ed to input_bytes[].
	*/
	byteindex = 0;
	for (i=0; i<20; i++){
		r1[i] ^=   (uint8_t)((key->rp5[byteindex])<<4)
				 | (uint8_t)(key->rp5[byteindex+1]&0x0F);
		byteindex += 2;
	}
	mqq_inv_affine_transformation(r1, (uint8_t*)dest, key);
}
