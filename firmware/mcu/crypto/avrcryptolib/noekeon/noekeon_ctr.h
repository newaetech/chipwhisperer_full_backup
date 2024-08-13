/* noekeon_ctr.h */
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
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2008-08-06
 * \license GPLv3 or later
 * 
 * 
 * 
 */
 
#ifndef NOEKEON_CTR_H_
#define NOEKEON_CTR_H_

#include <stdint.h>
#include "noekeon.h"

typedef struct{
	uint8_t counter[16];
	uint8_t key[16];
}noekeon_ctr_ctx_t;

void noekeon_ctr_next(void* buffer, const noekeon_ctr_ctx_t* ctx);

#endif /*NOEKEON_CTR_H_*/
