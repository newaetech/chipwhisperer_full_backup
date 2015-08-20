/* cast5-sbox.h */
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
/* 
 * File:	cast5-sbox.h
 * Author:	Daniel Otte
 * Date: 	26.07.2006
 * License: GPL
 * Description: sboxes for CAST5 (aka CAST-128) cipher algorithm as described in RFC 2144.
 * 
 */
 
#ifndef CAST5_SBOX_H_
#define CAST5_SBOX_H_

#include <stdint.h>

extern const uint32_t s1[];
extern const uint32_t s2[];
extern const uint32_t s3[];
extern const uint32_t s4[];
extern const uint32_t s5[];
extern const uint32_t s6[];
extern const uint32_t s7[];
extern const uint32_t s8[];

#endif

