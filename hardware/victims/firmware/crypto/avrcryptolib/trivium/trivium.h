/* trivium.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

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
#ifndef TRIVIUM_H_
#define TRIVIUM_H_

typedef uint8_t trivium_ctx_t[36]; /* 288bit */

uint8_t trivium_enc(trivium_ctx_t* ctx);
uint8_t trivium_getbyte(trivium_ctx_t* ctx);
void trivium_init(const void* key, uint16_t keysize_b,
                  const void* iv,  uint16_t ivsize_b,
                  trivium_ctx_t* ctx);

#endif /*TRIVIUM_H_*/
