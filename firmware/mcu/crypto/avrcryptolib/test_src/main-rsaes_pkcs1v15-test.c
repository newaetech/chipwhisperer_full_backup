/* main-dsa-test.c */
/*
    This file is part of the ARM-Crypto-Lib.
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
/*
 * RSA test-suit
 *
*/
#include "main-test-common.h"

#include "noekeon.h"
#include "noekeon_prng.h"
#include "bigint.h"
#include "bigint_io.h"
#include "random_dummy.h"
#include "rsa_basic.h"
#include "rsaes_pkcs1v15.h"

#include "performance_test.h"

#define DEBUG 1

const char* algo_name = "RSAES-PKCS1V15";

#define BIGINT_CEIL(x) ((((x) + sizeof(bigint_word_t) - 1) / sizeof(bigint_word_t)) *  sizeof(bigint_word_t))
#define BIGINT_OFF(x) ((sizeof(bigint_word_t) - (x) % sizeof(bigint_word_t)) % sizeof(bigint_word_t))

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

/* Modulus: */
const uint8_t modulus[] PROGMEM = {
0xa8, 0xb3, 0xb2, 0x84, 0xaf, 0x8e, 0xb5, 0x0b, 0x38, 0x70, 0x34, 0xa8, 0x60, 0xf1, 0x46, 0xc4,
0x91, 0x9f, 0x31, 0x87, 0x63, 0xcd, 0x6c, 0x55, 0x98, 0xc8, 0xae, 0x48, 0x11, 0xa1, 0xe0, 0xab,
0xc4, 0xc7, 0xe0, 0xb0, 0x82, 0xd6, 0x93, 0xa5, 0xe7, 0xfc, 0xed, 0x67, 0x5c, 0xf4, 0x66, 0x85,
0x12, 0x77, 0x2c, 0x0c, 0xbc, 0x64, 0xa7, 0x42, 0xc6, 0xc6, 0x30, 0xf5, 0x33, 0xc8, 0xcc, 0x72,
0xf6, 0x2a, 0xe8, 0x33, 0xc4, 0x0b, 0xf2, 0x58, 0x42, 0xe9, 0x84, 0xbb, 0x78, 0xbd, 0xbf, 0x97,
0xc0, 0x10, 0x7d, 0x55, 0xbd, 0xb6, 0x62, 0xf5, 0xc4, 0xe0, 0xfa, 0xb9, 0x84, 0x5c, 0xb5, 0x14,
0x8e, 0xf7, 0x39, 0x2d, 0xd3, 0xaa, 0xff, 0x93, 0xae, 0x1e, 0x6b, 0x66, 0x7b, 0xb3, 0xd4, 0x24,
0x76, 0x16, 0xd4, 0xf5, 0xba, 0x10, 0xd4, 0xcf, 0xd2, 0x26, 0xde, 0x88, 0xd3, 0x9f, 0x16, 0xfb
};

/* Public exponent: */
const uint8_t pub_exponent[] PROGMEM = { 0x01, 0x00, 0x01 };

/* Exponent: */
const uint8_t priv_exponent[] PROGMEM = {
0x53, 0x33, 0x9c, 0xfd, 0xb7, 0x9f, 0xc8, 0x46, 0x6a, 0x65, 0x5c, 0x73, 0x16, 0xac, 0xa8, 0x5c,
0x55, 0xfd, 0x8f, 0x6d, 0xd8, 0x98, 0xfd, 0xaf, 0x11, 0x95, 0x17, 0xef, 0x4f, 0x52, 0xe8, 0xfd,
0x8e, 0x25, 0x8d, 0xf9, 0x3f, 0xee, 0x18, 0x0f, 0xa0, 0xe4, 0xab, 0x29, 0x69, 0x3c, 0xd8, 0x3b,
0x15, 0x2a, 0x55, 0x3d, 0x4a, 0xc4, 0xd1, 0x81, 0x2b, 0x8b, 0x9f, 0xa5, 0xaf, 0x0e, 0x7f, 0x55,
0xfe, 0x73, 0x04, 0xdf, 0x41, 0x57, 0x09, 0x26, 0xf3, 0x31, 0x1f, 0x15, 0xc4, 0xd6, 0x5a, 0x73,
0x2c, 0x48, 0x31, 0x16, 0xee, 0x3d, 0x3d, 0x2d, 0x0a, 0xf3, 0x54, 0x9a, 0xd9, 0xbf, 0x7c, 0xbf,
0xb7, 0x8a, 0xd8, 0x84, 0xf8, 0x4d, 0x5b, 0xeb, 0x04, 0x72, 0x4d, 0xc7, 0x36, 0x9b, 0x31, 0xde,
0xf3, 0x7d, 0x0c, 0xf5, 0x39, 0xe9, 0xcf, 0xcd, 0xd3, 0xde, 0x65, 0x37, 0x29, 0xea, 0xd5, 0xd1
};

/* Prime 1: */
const uint8_t p[] PROGMEM = {
0xd3, 0x27, 0x37, 0xe7, 0x26, 0x7f, 0xfe, 0x13, 0x41, 0xb2, 0xd5, 0xc0, 0xd1, 0x50, 0xa8, 0x1b,
0x58, 0x6f, 0xb3, 0x13, 0x2b, 0xed, 0x2f, 0x8d, 0x52, 0x62, 0x86, 0x4a, 0x9c, 0xb9, 0xf3, 0x0a,
0xf3, 0x8b, 0xe4, 0x48, 0x59, 0x8d, 0x41, 0x3a, 0x17, 0x2e, 0xfb, 0x80, 0x2c, 0x21, 0xac, 0xf1,
0xc1, 0x1c, 0x52, 0x0c, 0x2f, 0x26, 0xa4, 0x71, 0xdc, 0xad, 0x21, 0x2e, 0xac, 0x7c, 0xa3, 0x9d
};

/* Prime 2: */
const uint8_t q[] PROGMEM = {
0xcc, 0x88, 0x53, 0xd1, 0xd5, 0x4d, 0xa6, 0x30, 0xfa, 0xc0, 0x04, 0xf4, 0x71, 0xf2, 0x81, 0xc7,
0xb8, 0x98, 0x2d, 0x82, 0x24, 0xa4, 0x90, 0xed, 0xbe, 0xb3, 0x3d, 0x3e, 0x3d, 0x5c, 0xc9, 0x3c,
0x47, 0x65, 0x70, 0x3d, 0x1d, 0xd7, 0x91, 0x64, 0x2f, 0x1f, 0x11, 0x6a, 0x0d, 0xd8, 0x52, 0xbe,
0x24, 0x19, 0xb2, 0xaf, 0x72, 0xbf, 0xe9, 0xa0, 0x30, 0xe8, 0x60, 0xb0, 0x28, 0x8b, 0x5d, 0x77
};

/* Prime exponent 1: */
const uint8_t dp[] PROGMEM = {
0x0e, 0x12, 0xbf, 0x17, 0x18, 0xe9, 0xce, 0xf5, 0x59, 0x9b, 0xa1, 0xc3, 0x88, 0x2f, 0xe8, 0x04,
0x6a, 0x90, 0x87, 0x4e, 0xef, 0xce, 0x8f, 0x2c, 0xcc, 0x20, 0xe4, 0xf2, 0x74, 0x1f, 0xb0, 0xa3,
0x3a, 0x38, 0x48, 0xae, 0xc9, 0xc9, 0x30, 0x5f, 0xbe, 0xcb, 0xd2, 0xd7, 0x68, 0x19, 0x96, 0x7d,
0x46, 0x71, 0xac, 0xc6, 0x43, 0x1e, 0x40, 0x37, 0x96, 0x8d, 0xb3, 0x78, 0x78, 0xe6, 0x95, 0xc1
};

/* Prime exponent 2: */
const uint8_t dq[] PROGMEM = {
0x95, 0x29, 0x7b, 0x0f, 0x95, 0xa2, 0xfa, 0x67, 0xd0, 0x07, 0x07, 0xd6, 0x09, 0xdf, 0xd4, 0xfc,
0x05, 0xc8, 0x9d, 0xaf, 0xc2, 0xef, 0x6d, 0x6e, 0xa5, 0x5b, 0xec, 0x77, 0x1e, 0xa3, 0x33, 0x73,
0x4d, 0x92, 0x51, 0xe7, 0x90, 0x82, 0xec, 0xda, 0x86, 0x6e, 0xfe, 0xf1, 0x3c, 0x45, 0x9e, 0x1a,
0x63, 0x13, 0x86, 0xb7, 0xe3, 0x54, 0xc8, 0x99, 0xf5, 0xf1, 0x12, 0xca, 0x85, 0xd7, 0x15, 0x83
};

/* Coefficient: */
const uint8_t qinv[] PROGMEM = {
0x4f, 0x45, 0x6c, 0x50, 0x24, 0x93, 0xbd, 0xc0, 0xed, 0x2a, 0xb7, 0x56, 0xa3, 0xa6, 0xed, 0x4d,
0x67, 0x35, 0x2a, 0x69, 0x7d, 0x42, 0x16, 0xe9, 0x32, 0x12, 0xb1, 0x27, 0xa6, 0x3d, 0x54, 0x11,
0xce, 0x6f, 0xa9, 0x8d, 0x5d, 0xbe, 0xfd, 0x73, 0x26, 0x3e, 0x37, 0x28, 0x14, 0x27, 0x43, 0x81,
0x81, 0x66, 0xed, 0x7d, 0xd6, 0x36, 0x87, 0xdd, 0x2a, 0x8c, 0xa1, 0xd2, 0xf4, 0xfb, 0xd8, 0xe1
};

/* PKCS#1 v1.5 encryption of 0x20, random messages with random s0xee,ds
 *  ---------------------------------------------------------------------------
 */

/* Message: */
const uint8_t message_x[] PROGMEM = {
0x66, 0x28, 0x19, 0x4e, 0x12, 0x07, 0x3d, 0xb0, 0x3b, 0xa9, 0x4c, 0xda, 0x9e, 0xf9, 0x53, 0x23,
0x97, 0xd5, 0x0d, 0xba, 0x79, 0xb9, 0x87, 0x00, 0x4a, 0xfe, 0xfe, 0x34
};

/* Seed: */
const uint8_t seed_x[] PROGMEM = {
0x01, 0x73, 0x41, 0xae, 0x38, 0x75, 0xd5, 0xf8, 0x71, 0x01, 0xf8, 0xcc, 0x4f, 0xa9, 0xb9, 0xbc,
0x15, 0x6b, 0xb0, 0x46, 0x28, 0xfc, 0xcd, 0xb2, 0xf4, 0xf1, 0x1e, 0x90, 0x5b, 0xd3, 0xa1, 0x55,
0xd3, 0x76, 0xf5, 0x93, 0xbd, 0x73, 0x04, 0x21, 0x08, 0x74, 0xeb, 0xa0, 0x8a, 0x5e, 0x22, 0xbc,
0xcc, 0xb4, 0xc9, 0xd3, 0x88, 0x2a, 0x93, 0xa5, 0x4d, 0xb0, 0x22, 0xf5, 0x03, 0xd1, 0x63, 0x38,
0xb6, 0xb7, 0xce, 0x16, 0xdc, 0x7f, 0x4b, 0xbf, 0x9a, 0x96, 0xb5, 0x97, 0x72, 0xd6, 0x60, 0x6e,
0x97, 0x47, 0xc7, 0x64, 0x9b, 0xf9, 0xe0, 0x83, 0xdb, 0x98, 0x18, 0x84, 0xa9, 0x54, 0xab, 0x3c,
0x6f };

/* Encryption: */
const uint8_t encrypted_x[] PROGMEM = {
0x50, 0xb4, 0xc1, 0x41, 0x36, 0xbd, 0x19, 0x8c, 0x2f, 0x3c, 0x3e, 0xd2, 0x43, 0xfc, 0xe0, 0x36,
0xe1, 0x68, 0xd5, 0x65, 0x17, 0x98, 0x4a, 0x26, 0x3c, 0xd6, 0x64, 0x92, 0xb8, 0x08, 0x04, 0xf1,
0x69, 0xd2, 0x10, 0xf2, 0xb9, 0xbd, 0xfb, 0x48, 0xb1, 0x2f, 0x9e, 0xa0, 0x50, 0x09, 0xc7, 0x7d,
0xa2, 0x57, 0xcc, 0x60, 0x0c, 0xce, 0xfe, 0x3a, 0x62, 0x83, 0x78, 0x9d, 0x8e, 0xa0, 0xe6, 0x07,
0xac, 0x58, 0xe2, 0x69, 0x0e, 0xc4, 0xeb, 0xc1, 0x01, 0x46, 0xe8, 0xcb, 0xaa, 0x5e, 0xd4, 0xd5,
0xcc, 0xe6, 0xfe, 0x7b, 0x0f, 0xf9, 0xef, 0xc1, 0xea, 0xbb, 0x56, 0x4d, 0xbf, 0x49, 0x82, 0x85,
0xf4, 0x49, 0xee, 0x61, 0xdd, 0x7b, 0x42, 0xee, 0x5b, 0x58, 0x92, 0xcb, 0x90, 0x60, 0x1f, 0x30,
0xcd, 0xa0, 0x7b, 0xf2, 0x64, 0x89, 0x31, 0x0b, 0xcd, 0x23, 0xb5, 0x28, 0xce, 0xab, 0x3c, 0x31
};

uint8_t keys_allocated = 0;
rsa_publickey_t pub_key;
rsa_privatekey_t priv_key;

#if 1
  #define MSG       message_x
  #define SEED      seed_x
  #define ENCRYPTED encrypted_x
  #define MODULUS modulus
  #define PUB_EXPONENT pub_exponent
  #define PRIV_EXPONENT priv_exponent
  #define P p
  #define Q q
  #define DP dp
  #define DQ dq
  #define QINV qinv
#endif


uint8_t convert_nibble(uint8_t c){
	if(c>='0' && c<='9'){
		return c - '0';
	}
	c |= 'A' ^ 'a';
	if(c>='a' && c<='f'){
		return c - 'a' + 10;
	}
	return 0xff;
}

const char *block_ignore_string=" \t\r\n,;";
#define BUFFER_LIMIT 120
uint16_t read_os(void* dst, uint16_t length, const char* ignore_string){
	uint16_t counter = 0;
	uint16_t c;
	uint8_t v, tmp = 0, idx = 0;
	if(!ignore_string){
		ignore_string = block_ignore_string;
	}
	while(counter < length){
		c = cli_getc();
		if(c > 0xff){
			return counter;
		}
		if(strchr(ignore_string, c)){
			continue;
		}
		v = convert_nibble(c);
		if(v > 0x0f){
			return counter;
		}
		if(idx){
			((uint8_t*)dst)[counter++] = (tmp << 4) | v;
			idx = 0;
			if(counter % (BUFFER_LIMIT/2) == 0){
				cli_putc('.');
			}
		}else{
			tmp = v;
			idx = 1;
		}
	}
	return counter;
}

uint16_t own_atou(const char* str){
	uint16_t r=0;
	while(*str && *str >= '0' && *str <= '9'){
		r *= 10;
		r += *str++ - '0';
	}
	return r;
}

uint8_t read_bigint(bigint_t* a, char* prompt){
	uint16_t read_length, actual_length;
	uint8_t off;
	uint8_t *buffer;
	char read_int_str[18];
	cli_putstr(prompt);
	cli_putstr_P(PSTR("\r\n  length: "));
	cli_getsn(read_int_str, 16);
	read_length = own_atou(read_int_str);
	off = (sizeof(bigint_word_t) - (read_length % sizeof(bigint_word_t))) % sizeof(bigint_word_t);
	buffer = malloc(((read_length + sizeof(bigint_word_t) - 1) / sizeof(bigint_word_t)) * sizeof(bigint_word_t));
	if(!buffer){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return 2;
	}
	cli_putstr_P(PSTR("\r\n  data: "));
	memset(buffer, 0, sizeof(bigint_word_t));
	actual_length = read_os(buffer + off, read_length, NULL);
	if(actual_length != read_length){
		cli_putstr_P(PSTR("\r\nERROR: unexpected end of data!"));
		free(buffer);
		return 1;
	}
	a->wordv = (bigint_word_t*)buffer;
	a->length_B = (read_length + sizeof(bigint_word_t) - 1) / sizeof(bigint_word_t);
	a->info = 0;
	bigint_changeendianess(a);
	bigint_adjust(a);
	return 0;
}

uint8_t pre_alloc_key_crt(void){
	priv_key.n = 5;
	priv_key.components = malloc(5 * sizeof(bigint_t));
	if(!priv_key.components){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return 2;
	}
	return 0;
}

void free_key(void){
	uint8_t c;
	free(pub_key.modulus.wordv);
	free(pub_key.exponent.wordv);
	for(c = 0; c < priv_key.n; ++c){
		free(priv_key.components[c].wordv);
	}
	free(priv_key.components);
	keys_allocated = 0;
}

uint8_t read_key_crt(void){
	uint8_t r;
	cli_putstr_P(PSTR("\r\n== reading key (crt) =="));
	r = pre_alloc_key_crt();
	if(r) return r;
	r = read_bigint(&pub_key.modulus,"\r\n = module =");
	if(r) return r;
	memcpy(&priv_key.modulus, &pub_key.modulus, sizeof(bigint_t));
	r = read_bigint(&pub_key.exponent,"\r\n = public exponent =");
	if(r) return r;
	r = read_bigint(&(priv_key.components[0]),"\r\n = p (first prime) =");
	if(r) return r;
	r = read_bigint(&(priv_key.components[1]),"\r\n = q (second prime) =");
	if(r) return r;
	r = read_bigint(&(priv_key.components[2]),"\r\n = dp (p's exponent) =");
	if(r) return r;
	r = read_bigint(&(priv_key.components[3]),"\r\n = dq (q's exponent) =");
	if(r) return r;
	r = read_bigint(&(priv_key.components[4]),"\r\n = qInv (q' coefficient) =");
	return r;
}

uint8_t read_key_conv(void){
	uint8_t r;
	priv_key.components = malloc(sizeof(bigint_t));
	if(!priv_key.components){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return 2;
	}
	cli_putstr_P(PSTR("\r\n== reading key (conv) =="));
	r = read_bigint(&pub_key.modulus,"\r\n = module =");
	if(r) return r;
	memcpy(&priv_key.modulus, &pub_key.modulus, sizeof(bigint_t));
	priv_key.n = 1;
	r = read_bigint(&pub_key.exponent,"\r\n = public exponent =");
	if(r) return r;
	r = read_bigint(priv_key.components,"\r\n = private exponent =");
	return r;
}

uint8_t load_bigint_from_os(bigint_t* a, PGM_VOID_P os, uint16_t length_B){
	a->length_B = BIGINT_CEIL(length_B) / sizeof(bigint_word_t);
	a->wordv = malloc(BIGINT_CEIL(length_B));
	if(!a->wordv){
		cli_putstr_P(PSTR("\r\nOOM!\r\n"));
		return 1;
	}
	memset(a->wordv, 0, sizeof(bigint_word_t));
	memcpy_P((uint8_t*)a->wordv + BIGINT_OFF(length_B), os, length_B);
	a->info = 0;
	bigint_changeendianess(a);
	bigint_adjust(a);
	return 0;
}

void load_fix_rsa(void){
	if(keys_allocated){
		free_key();
	}
	keys_allocated = 1;

	if(pre_alloc_key_crt()){
		cli_putstr_P(PSTR("\r\nOOM!\r\n"));
		return;
	}

	load_bigint_from_os(&pub_key.modulus, MODULUS, sizeof(MODULUS));
	memcpy(&priv_key.modulus, &pub_key.modulus, sizeof(bigint_t));
	load_bigint_from_os(&pub_key.exponent, PUB_EXPONENT, sizeof(PUB_EXPONENT));
	priv_key.n = 5;
	load_bigint_from_os(&(priv_key.components[0]), P, sizeof(P));
	load_bigint_from_os(&(priv_key.components[1]), Q, sizeof(Q));
	load_bigint_from_os(&(priv_key.components[2]), DP, sizeof(DP));
	load_bigint_from_os(&(priv_key.components[3]), DQ, sizeof(DQ));
	load_bigint_from_os(&(priv_key.components[4]), QINV, sizeof(QINV));
}

/*

uint8_t pre_alloc_key_crt(void){
	uint8_t c;
	pub_key.modulus = malloc(sizeof(bigint_t));
	if(!pub_key.modulus){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return 6;
	}
	priv_key.modulus = pub_key.modulus;
	priv_key.n = 5;
	priv_key.components = malloc(5 * sizeof(bigint_t*));
	if(!priv_key.components){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return 3;
	}
	pub_key.exponent = malloc(sizeof(bigint_t));
	if(!pub_key.exponent){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return 4;
	}
	for(c=0; c<5; ++c){
		priv_key.components[c] = malloc(sizeof(bigint_t));
		if(!priv_key.components[c]){
			cli_putstr_P(PSTR("\r\nERROR: OOM!"));
			return 7+c;
		}
	}
	return 0;
}

void free_key(void){
	uint8_t c;
	for(c = priv_key.n; c > 0 ; --c){
		free(priv_key.components[c - 1]->wordv);
	}
	free(pub_key.exponent->wordv);
	free(pub_key.modulus->wordv);

	for(c = priv_key.n; c > 0 ; --c){
		free(priv_key.components[c - 1]);
	}
	free(pub_key.exponent);
	pub_key.exponent = NULL;
	free(priv_key.components);
	priv_key.components = NULL;
	free(pub_key.modulus);
	pub_key.modulus = priv_key.modulus = NULL;
	keys_allocated = 0;
}

uint8_t read_key_crt(void){
	uint8_t r;
	cli_putstr_P(PSTR("\r\n== reading key (crt) =="));
	r = pre_alloc_key_crt();
	if(r) return r;
	r = read_bigint(pub_key.modulus,"\r\n = module =");
	if(r) return r;
	r = read_bigint(pub_key.exponent,"\r\n = public exponent =");
	if(r) return r;
	r = read_bigint(priv_key.components[0],"\r\n = p (first prime) =");
	if(r) return r;
	r = read_bigint(priv_key.components[1],"\r\n = q (second prime) =");
	if(r) return r;
	r = read_bigint(priv_key.components[2],"\r\n = dp (p's exponent) =");
	if(r) return r;
	r = read_bigint(priv_key.components[3],"\r\n = dq (q's exponent) =");
	if(r) return r;
	r = read_bigint(priv_key.components[4],"\r\n = qInv (q' coefficient) =");

#if DEBUG
	cli_putstr_P(PSTR("\r\nmodulus:"));
	bigint_print_hex(pub_key.modulus);
	cli_putstr_P(PSTR("\r\npublic exponent:"));
	bigint_print_hex(pub_key.exponent);
	cli_putstr_P(PSTR("\r\np:"));
	bigint_print_hex(priv_key.components[0]);
	cli_putstr_P(PSTR("\r\nq:"));
	bigint_print_hex(priv_key.components[1]);
	cli_putstr_P(PSTR("\r\ndP:"));
	bigint_print_hex(priv_key.components[2]);
	cli_putstr_P(PSTR("\r\ndQ:"));
	bigint_print_hex(priv_key.components[3]);
	cli_putstr_P(PSTR("\r\nqInv:"));
	bigint_print_hex(priv_key.components[4]);
#endif
	return r;
}

uint8_t read_key_conv(void){
	uint8_t r;
	cli_putstr_P(PSTR("\r\n== reading key (crt) =="));
	pub_key.modulus = malloc(sizeof(bigint_t));
	if(!pub_key.modulus){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return 2;
	}
	r = read_bigint(pub_key.modulus,"\r\n = module =");
	if(r) return r;
	priv_key.modulus = pub_key.modulus;
	priv_key.n = 1;
	pub_key.exponent = malloc(sizeof(bigint_t));
	if(!pub_key.exponent){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return 2;
	}
	priv_key.components = malloc(sizeof(bigint_t*));
	if(!priv_key.components){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return 2;
	}
	priv_key.components[0] = malloc(sizeof(bigint_t));
	if(!priv_key.components[0]){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return 2;
	}
	r = read_bigint(pub_key.exponent,"\r\n = public exponent =");
	if(r) return r;
	r = read_bigint(priv_key.components[0],"\r\n = private exponent =");
	return r;
}

void load_priv_conventional(void){
	bigint_t *epriv;
	epriv = malloc(sizeof(bigint_t));
	if(!epriv){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return;
	}
	epriv->length_B = (sizeof(PRIV_EXPONENT) + sizeof(bigint_word_t) - 1) / sizeof(bigint_word_t);
	epriv->wordv =  malloc(epriv->length_B * sizeof(bigint_word_t));
	if(!epriv->wordv){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return;
	}
	memcpy(epriv->wordv, PRIV_EXPONENT, sizeof(PRIV_EXPONENT));
	priv_key.components = malloc(sizeof(bigint_t*));
	priv_key.components[0] = epriv;
	priv_key.n = 1;
	bigint_changeendianess(epriv);
	bigint_adjust(epriv);
}


void load_priv_crt_mono(void){
	bigint_t **v;
	const uint8_t *bv[5] = {P,Q,DP,DQ,QINV};
	uint16_t sv[5] = {sizeof(P), sizeof(Q), sizeof(DP), sizeof(DQ), sizeof(QINV)};
	uint8_t i;
	v = malloc(5 * sizeof(bigint_t));
	if(!v){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return;
	}
	priv_key.components = malloc(5*sizeof(bigint_t*));
	if(!priv_key.components){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return;
	}
	priv_key.n = 5;
	for(i=0; i<5; ++i){
		v[i] = malloc(sizeof(bigint_t));
		v[i]->info = 0;
		v[i]->length_B = (sv[i] + sizeof(bigint_word_t) - 1) / sizeof(bigint_word_t);
		v[i]->wordv = calloc(v[i]->length_B , sizeof(bigint_word_t));
		if(!v[i]->wordv){
			cli_putstr_P(PSTR("\r\nERROR: OOM!"));
			return;
		}
		memcpy(v[i]->wordv, bv[i], sv[i]);
		bigint_changeendianess(v[i]);
		bigint_adjust(v[i]);
		priv_key.components[i] = v[i];
	}
}

uint8_t load_bigint_from_os(bigint_t* a, PGM_VOID_P os, uint16_t length_B){
	a->length_B = BIGINT_CEIL(length_B) / sizeof(bigint_word_t);
	a->wordv = malloc(BIGINT_CEIL(length_B));
	if(!a->wordv){
		cli_putstr_P(PSTR("\r\nOOM!\r\n"));
		return 1;
	}
	memset(a->wordv, 0, sizeof(bigint_word_t));
	memcpy_P((uint8_t*)a->wordv + BIGINT_OFF(length_B), os, length_B);
	a->info = 0;
	bigint_changeendianess(a);
	bigint_adjust(a);
	return 0;
}

void load_fix_rsa(void){
	if(keys_allocated){
		free_key();
	}
	keys_allocated = 1;

	if(pre_alloc_key_crt()){
		cli_putstr_P(PSTR("\r\nOOM!\r\n"));
		return;
	}

	load_bigint_from_os(pub_key.modulus, MODULUS, sizeof(MODULUS));
	load_bigint_from_os(pub_key.exponent, PUB_EXPONENT, sizeof(PUB_EXPONENT));
	priv_key.n = 5;
	load_bigint_from_os(priv_key.components[0], P, sizeof(P));
	load_bigint_from_os(priv_key.components[1], Q, sizeof(Q));
	load_bigint_from_os(priv_key.components[2], DP, sizeof(DP));
	load_bigint_from_os(priv_key.components[3], DQ, sizeof(DQ));
	load_bigint_from_os(priv_key.components[4], QINV, sizeof(QINV));

//	load_priv_conventional();
//	load_priv_crt_mono();
}
*/

void quick_test(void){
	uint8_t *ciphertext, *plaintext, rc;
	uint8_t seed[sizeof(SEED)], seed_out[sizeof(SEED)];
	uint16_t clen, plen;
	if(!keys_allocated){
		load_fix_rsa();
	}
	ciphertext = malloc(clen = bigint_length_B(&pub_key.modulus));
	plaintext = malloc(clen);
	memcpy_P(plaintext, MSG, sizeof(MSG));
	memcpy_P(seed, SEED, sizeof(SEED));
	cli_putstr_P(PSTR("\r\nplaintext:"));
	cli_hexdump_block(plaintext, sizeof(MSG), 4, 16);
	cli_putstr_P(PSTR("\r\nseed:"));
	cli_hexdump_block(seed, sizeof(SEED), 4, 16);
	cli_putstr_P(PSTR("\r\nencrypting: ..."));

	rc = rsa_encrypt_pkcs1v15(ciphertext, &clen, plaintext, sizeof(MSG), &pub_key, seed);
	if(rc){
		cli_putstr_P(PSTR("\r\nERROR: rsa_encrypt_pkcs1v15 returned: "));
		cli_hexdump_byte(rc);
		return;

	}

	cli_putstr_P(PSTR("\r\n\r\nciphertext:"));
	cli_hexdump_block(ciphertext, clen, 4, 16);
	if(clen!=sizeof(ENCRYPTED)){
			cli_putstr_P(PSTR("\r\n>>FAIL (no size match)<<"));
	}else{
		if(memcmp_P(ciphertext, ENCRYPTED, clen)){
			cli_putstr_P(PSTR("\r\n>>FAIL (no content match)<<"));
		}else{
			cli_putstr_P(PSTR("\r\n>>OK<<"));
		}
	}

	cli_putstr_P(PSTR("\r\ndecrypting: ..."));
	rc = rsa_decrypt_pkcs1v15(plaintext, &plen, ciphertext, clen, &priv_key, seed_out);
	if(rc){
		cli_putstr_P(PSTR("\r\nERROR: rsa_decrypt_pkcs1v15 returned: "));
		cli_hexdump_byte(rc);
		return;
	}
	cli_putstr_P(PSTR("\r\n\r\nplaintext:"));
	cli_hexdump_block(plaintext, plen, 4, 16);
	cli_putstr_P(PSTR("\r\n\r\nseed (out):"));
	cli_hexdump_block(seed_out, sizeof(SEED), 4, 16);

	free(ciphertext);
	free(plaintext);
}

void run_seed_test(void){
	uint8_t *msg, *ciph, *msg_;
	uint16_t ciph_len, msg_len;
	uint16_t msg_len_;
	uint16_t seed_len;
	uint8_t *seed, *seed_out;
	char read_int_str[18];
	cli_putstr_P(PSTR("\r\n== test with given seed =="));
	cli_putstr_P(PSTR("\r\n = message ="));
	cli_putstr_P(PSTR("\r\n  length: "));
	cli_getsn(read_int_str, 16);
	msg_len = own_atou(read_int_str);
	seed_len = rsa_pkcs1v15_compute_padlength_B(&pub_key.modulus, msg_len);
	seed = malloc(seed_len);
#if DEBUG
	cli_putstr_P(PSTR("\r\nDBG: @seed: 0x"));
	cli_hexdump_rev(&seed, 2);
#endif
	if(!seed){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return;
	}
	seed_out = malloc(seed_len);
#if DEBUG
	cli_putstr_P(PSTR("\r\nDBG: @seed_out: 0x"));
	cli_hexdump_rev(&seed_out, 2);
#endif
	if(!seed_out){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return;
	}
	msg = malloc(msg_len);
#if DEBUG
	cli_putstr_P(PSTR("\r\nDBG: @msg: 0x"));
	cli_hexdump_rev(&msg, 2);
#endif
	if(!msg){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return;
	}
	ciph = malloc(bigint_length_B(&pub_key.modulus));
#if DEBUG
	cli_putstr_P(PSTR("\r\nDBG: @ciph: 0x"));
	cli_hexdump_rev(&ciph, 2);
#endif
	if(!ciph){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return;
	}
	msg_ = malloc(bigint_length_B(&pub_key.modulus) /* + sizeof(bigint_word_t) */ );
#if DEBUG
	cli_putstr_P(PSTR("\r\nDBG: @msg_: 0x"));
	cli_hexdump_rev(&msg_, 2);
#endif
	if(!msg_){
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
		return;
	}
	cli_putstr_P(PSTR("\r\n  data: "));
	read_os(msg, msg_len, NULL);
	cli_putstr_P(PSTR("\r\n  seed (0x"));
	cli_hexdump_rev(&seed_len, 2);
	cli_putstr_P(PSTR(" bytes): "));
	read_os(seed, seed_len, NULL);

	cli_putstr_P(PSTR("\r\n  encrypting ..."));
/*
	cli_putstr_P(PSTR("\r\n plaintext:"));
	cli_hexdump_block(msg, msg_len, 4, 16);
	cli_putstr_P(PSTR("\r\n seed:"));
	cli_hexdump_block(seed, seed_len, 4, 16);
*/
#if DEBUG
	cli_putstr_P(PSTR("\r\n  first prime:"));
	bigint_print_hex(&priv_key.components[0]);
#endif
	rsa_encrypt_pkcs1v15(ciph, &ciph_len, msg, msg_len, &pub_key, seed);
	cli_putstr_P(PSTR("\r\n  ciphertext:"));
	cli_hexdump_block(ciph, ciph_len, 4, 16);
#if DEBUG
	cli_putstr_P(PSTR("\r\n  first prime:"));
	bigint_print_hex(&priv_key.components[0]);
#endif
	cli_putstr_P(PSTR("\r\n  decrypting ... "));

	rsa_decrypt_pkcs1v15(msg_, &msg_len_, ciph, ciph_len, &priv_key, seed_out);

	cli_putstr_P(PSTR("[done]"));
	if(msg_len != msg_len_){
		char tstr[16];
		cli_putstr_P(PSTR("\r\nERROR: wrong decrypted message length ("));
		itoa(msg_len_, tstr, 10);
		cli_putstr(tstr);
		cli_putstr_P(PSTR(" instead of "));
		itoa(msg_len, tstr, 10);
		cli_putstr(tstr);
		cli_putc(')');
		goto end;
	}
	if(memcmp(msg, msg_, msg_len)){
		cli_putstr_P(PSTR("\r\nERROR: wrong decrypted message:"));
		cli_hexdump_block(msg_, msg_len_, 4, 16);
		cli_putstr_P(PSTR("\r\nreference:"));
		cli_hexdump_block(msg, msg_len, 4, 16);
		goto end;
	}

	if(memcmp(seed, seed_out, seed_len)){
		cli_putstr_P(PSTR("\r\nERROR: wrong decrypted seed:"));
		cli_hexdump_block(seed_out, seed_len, 4, 16);
		cli_putstr_P(PSTR("\r\nreference:"));
		cli_hexdump_block(seed, seed_len, 4, 16);
		goto end;
	}

	cli_putstr_P(PSTR("\r\n  >>OK<<"));
end:
	free(msg_);
	free(ciph);
	free(msg);
	free(seed_out);
	free(seed);
}

void reset_prng(void){
	uint8_t buf[16];
	memset(buf, 0, 16);
	random_seed(buf);
	cli_putstr_P(PSTR("\r\nPRNG reset"));
}

void rsa_init(void){
	prng_get_byte = random8;
}

void load_key(void){
	uint8_t r;
	if(keys_allocated){
		cli_putstr_P(PSTR("\r\nDBG: freeing old keys"));
		free_key();
	}
	keys_allocated = 1;
	r = read_key_crt();
	if(r){
		cli_putstr_P(PSTR("\r\nERROR: read_key_crt returned 0x"));
		cli_hexdump_byte(r);
	}
}

void test_dump(void){
	char lstr[16];
	int len;
	cli_putstr_P(PSTR("\r\nenter dump length: "));
	cli_getsn_cecho(lstr, 15);
	len = own_atou(lstr);
	cli_putstr_P(PSTR("\r\ndumping 0x"));
	cli_hexdump_rev(&len, 2);
	cli_putstr_P(PSTR(" byte:"));
	cli_hexdump_block(pub_key.modulus.wordv, len, 4, 8);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char echo_test_str[]    PROGMEM = "echo-test";
const char reset_prng_str[]   PROGMEM = "reset-prng";
const char load_key_str[]     PROGMEM = "load-key";
const char load_fix_key_str[] PROGMEM = "load-fix-key";
const char quick_test_str[]   PROGMEM = "quick-test";
const char seed_test_str[]    PROGMEM = "seed-test";
const char dump_test_str[]    PROGMEM = "dump-test";
const char performance_str[]  PROGMEM = "performance";
const char echo_str[]         PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ reset_prng_str,       NULL, reset_prng                    },
	{ load_key_str,         NULL, load_key                      },
	{ load_fix_key_str,     NULL, load_fix_rsa                  },
	{ quick_test_str,       NULL, quick_test                    },
	{ seed_test_str,        NULL, run_seed_test                 },
	{ dump_test_str,        NULL, test_dump                     },
//	{ performance_str,      NULL, testrun_performance_bigint    },
	{ echo_str,         (void*)1, (void_fpt)echo_ctrl           },
	{ NULL,                 NULL, NULL                          }
};

void dump_sp(void){
	uint8_t x;
	uint8_t *xa = &x;
	cli_putstr_P(PSTR("\r\nstack pointer: ~"));
	cli_hexdump_rev(&xa, 4);
}

int main (void){
	main_setup();

	for(;;){
		welcome_msg(algo_name);
		rsa_init();
		cmd_interface(cmdlist);
	}
}
