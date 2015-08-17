/* rsa_pkcs1v15.h */
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

#ifndef RSA_PKCS1V15_H_
#define RSA_PKCS1V15_H_

#include <stdint.h>
#include "bigint.h"

uint16_t rsa_pkcs1v15_compute_padlength_B(bigint_t* modulus, uint16_t msg_length_B);


uint8_t rsa_encrypt_pkcs1v15(void* dest, uint16_t* out_length, const void* src,
		uint16_t length_B, rsa_publickey_t* key, const void* pad);

uint8_t rsa_decrypt_pkcs1v15(void* dest, uint16_t* out_length, const void* src,
		uint16_t length_B, rsa_privatekey_t* key, void* pad);

#endif /* RSA_PKCS1V15_H_ */
