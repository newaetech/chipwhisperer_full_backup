/* mqq160-sign_testkey.c */
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

#include <stdint.h>
#include <avr/pgmspace.h>
#include "mqq160-sign.h"
#include "mqq160-sign_testkey.h"

/*
This is the private key of MQQ defined by one
quadratic quasigroup of order 2^8 given as 81 uint8_ts
and one nonsingular matrix SInv given as two arrays
RP1[] and RP5[] of 160 uint8_ts.
*/


const uint8_t a[9*9-1] PROGMEM =
 { 171, 171, 165,  56, 121, 136,  79, 108,   2,
   255, 255, 165, 108,  45, 220,  79, 108,  88,
    54, 108, 103,  21,  74, 119, 141, 204, 221,
   210, 220,  30, 201, 215, 199,  74,  95, 173,
   165, 241, 160, 190,  38, 134,  68, 103, 140,
    84,  84,  68, 157,  81,  65,  30,  11,  48,
   136, 136,  79,  21, 136, 199,  79,   0, 171,
   136, 210,  27,  27, 220, 157,  65,  84,  45,
   225,  61,   8, 232, 235,  49,  22, 146 };


const uint8_t cc1[9] PROGMEM = {  2,  88, 221, 173, 140,  48, 171,  45, 252 };
const uint8_t cc2[9] PROGMEM = {225,  61,   8, 232, 235,  49,  22, 146, 252 };

/* The matrix SInv is stored in ROM as two onedimensional
arrays RP1[] and RP5[] of 160 uint8_ts */
const uint8_t rp1[160] PROGMEM = {
	111, 137,  49, 134,   9, 116,  11,  52,  43,  55,
	 74, 130, 119, 144,  31,   7,  72,  79, 105,  59,
	 57, 120,  50,  94, 141, 135, 149,  44, 109, 100,
	113,   1, 143, 126, 117,  37,  65,  67, 152, 107,
	 10,  98,  15,  23, 138,  19, 121,  18,  28, 156,
	123, 106,  48,  29,  97,  34,  85, 157,  64,   3,
	 60,  35,  24,  32, 108, 147, 158,  21, 129,  84,
	  5,  70, 118, 112,  30,  68,  47,  40, 150,  13,
	 61,  73, 132,  22,  95, 153,   4,  76,  87, 114,
	127,  62,  27,  36, 125,  45, 142,  39, 101,  63,
	 88,  96,  12, 115,  82,  91, 159,  93, 155, 154,
	148, 110,  25,   0,  41,  20,  54,  26,  14,  83,
	 81,  80, 131,  33,  78,  77, 124, 104, 133,  17,
	145, 139, 122, 102,  42,  56,  75,  66,   2,  16,
	 86, 140,  71, 136,  69,  99,  58,   6,  92,  90,
	  8, 103, 128,  38,  46, 146,  89, 151,  51,  53 };

const uint8_t rp5[160] PROGMEM = {
	 90, 113, 130, 115, 132,  27,  46,  72,  33,  50,
	 35, 136,  42, 148, 146, 143, 116, 158,  98,  41,
	 39,   5,  54,  86, 106,  56,  30, 138,  80,  44,
	 91,  49,   1, 149, 159, 101,  74,   9, 110, 131,
	 25,  51, 123,  76, 104,  28,  82, 140,   2, 108,
	120, 144,  10, 145, 124, 119,  62,  57, 117, 121,
	 17,  73, 105,  69, 155,   7, 154,  75, 100, 141,
	157,  38,  14,  60,  47, 112,  95,  85,  43,  93,
	 24,  12,   4,  71,  81,  13,  94,  68, 107,  67,
	142, 150,  61,   6, 122,  26, 139,  59, 102, 153,
	109,  48, 103,  65,  23,  92,  87,  40, 135, 133,
	129, 134,   8,  55,  83, 125,  31,  96, 147,  36,
	  0, 126,  70,  64,  20,  11, 137,  78,  89,  58,
	 21, 114, 127, 111,  99,  34, 152,  79,  66,  97,
	 22,  15, 151,  32,  84,  37,  77,  88,  16,  29,
	  3, 128, 118,  18, 156,  19,  52,  45,  53,  63 };

const mqq160_sign_key_t testkey_P PROGMEM = {a, cc1, cc2, rp1, rp5 };

void mqq_load_pgm_key(void* buffer, mqq160_sign_key_t* key, const mqq160_sign_key_t* key_P){
	uint8_t *buf_ptr;
	buf_ptr = buffer;
	memcpy_P(key, key_P, sizeof(mqq160_sign_key_t));

	memcpy_P(buf_ptr, key->a, MQQ160SIGN_A_SIZE);
	key->a = buf_ptr;
	buf_ptr += MQQ160SIGN_A_SIZE;

	memcpy_P(buf_ptr, key->cc1, MQQ160SIGN_CC1_SIZE);
	key->cc1 = buf_ptr;
	buf_ptr += MQQ160SIGN_CC1_SIZE;

	memcpy_P(buf_ptr, key->cc2, MQQ160SIGN_CC2_SIZE);
	key->cc2 = buf_ptr;
	buf_ptr += MQQ160SIGN_CC2_SIZE;

	memcpy_P(buf_ptr, key->rp1, MQQ160SIGN_RP1_SIZE);
	key->rp1 = buf_ptr;
	buf_ptr += MQQ160SIGN_RP1_SIZE;

	memcpy_P(buf_ptr, key->rp5, MQQ160SIGN_RP5_SIZE);
	key->rp5 = buf_ptr;

}

