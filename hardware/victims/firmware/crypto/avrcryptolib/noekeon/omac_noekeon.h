/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008, 2009  Daniel Otte (daniel.otte@rub.de)

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
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \license GPLv3 or later
 * 
 * 
 * 
 */

#ifndef NOEKEON_OMAC_H_
#define NOEKEON_OMAC_H_

#include "noekeon.h"
#include <stdint.h>

typedef uint8_t omac_noekeon_ctx_t[16];

void omac_noekeon_init(omac_noekeon_ctx_t* ctx);
void omac_noekeon_tweak(uint8_t t, const void* key, omac_noekeon_ctx_t* ctx);
void omac_noekeon_next(const void* buffer, const void* key, 
                       omac_noekeon_ctx_t* ctx);
void omac_noekeon_last(const void* buffer, uint8_t length_b, const void* key, 
                       omac_noekeon_ctx_t* ctx);
void omac_noekeon(void* dest, const void* msg, uint16_t msglength_b,
                  const void* key, uint8_t t);

#endif /*NOEKEON_OMAC_H_*/
