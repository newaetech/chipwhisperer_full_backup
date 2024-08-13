/* rsa_oaep.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2012 Daniel Otte (daniel.otte@rub.de)

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

#ifndef RSA_OAEP_H_
#define RSA_OAEP_H_

#include <stdint.h>
#include "mgf1.h"


void mgf1(void* dest, const void* seed, uint16_t seed_len_B, uint16_t out_length_B, const mgf1_parameter_t* p);


typedef struct {
	void (*mgf)(void* dst, const void* seed, uint16_t slen_B, uint16_t dstlen_B, const mgf1_parameter_t* p);
	const hfdesc_t* hf;
	mgf1_parameter_t* mgf_parameter;
} rsa_oaep_parameter_t;

typedef struct {
	uint16_t length_b;
	const void* label;
} rsa_label_t;

extern rsa_oaep_parameter_t rsa_oaep_default_parameter;
extern rsa_label_t rsa_oaep_default_label;

uint8_t rsa_encrypt_oaep(void* dest, uint16_t* out_length,
		              const void* src, uint16_t length_B,
		              rsa_publickey_t* key, const rsa_oaep_parameter_t *p,
		              const rsa_label_t* label, const void* seed);

uint8_t rsa_decrypt_oaep(void* dest, uint16_t* out_length,
		              const void* src, uint16_t length_B,
		              rsa_privatekey_t* key, const rsa_oaep_parameter_t *p,
		              const rsa_label_t* label, void* seed);

#endif /* RSA_OAEP_H_ */
