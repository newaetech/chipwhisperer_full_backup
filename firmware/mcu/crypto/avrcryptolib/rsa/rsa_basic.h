/* rsa_basic.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2011 Daniel Otte (daniel.otte@rub.de)

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

#ifndef RSA_BASIC_H_
#define RSA_BASIC_H_

#include "bigint.h"

typedef struct {
	bigint_t exponent;
	bigint_t modulus;
} rsa_publickey_t;

typedef struct {
	uint8_t n;
	bigint_t modulus;
	bigint_t* components;
} rsa_privatekey_t;


typedef struct {
	rsa_privatekey_t priv;
	rsa_publickey_t  pub;
} rsa_fullkey_t;


void    rsa_enc(bigint_t* data, const rsa_publickey_t* key);
uint8_t rsa_dec(bigint_t* data, const rsa_privatekey_t* key);
void rsa_os2ip(bigint_t* dest, const void* data, uint32_t length_B);
void rsa_i2osp(void* dest, bigint_t* src, uint16_t* out_length_B);

#endif /* RSA_BASIC_H_ */
