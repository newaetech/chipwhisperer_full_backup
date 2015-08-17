/* cscipher.h */
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

#ifndef CSCIPHER_H_
#define CSCIPHER_H_

typedef struct {
	uint8_t keys[9][8];
} cscipher_ctx_t;

void cscipher_enc(void* buffer, const cscipher_ctx_t* ctx);
void cscipher_dec(void* buffer, const cscipher_ctx_t* ctx);
void cscipher_init(const void* key, cscipher_ctx_t* ctx);


#endif /* CSCIPHER_H_ */
