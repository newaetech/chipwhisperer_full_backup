/*
    This file is part of the AESExplorer Example Targets
    Copyright (C) 2012 Colin O'Flynn <coflynn@newae.com>

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

#include "aes-independant.h"

#ifdef AVRCRYPTOLIB
#include "aes128_enc.h"
#include "aes_keyschedule.h"

aes128_ctx_t ctx;

void aes_indep_init(void)
{
	;
}

void aes_indep_key(uint8_t * key)
{
	aes128_init(key, &ctx);
}

void aes_indep_enc(uint8_t * pt)
{
	aes128_enc(pt, &ctx); /* encrypting the data block */
}

#elif defined(SIMPLEAES)

uint8_t enckey[16];

void aes_indep_init(void)
{
	;
}

void aes_indep_key(uint8_t * key)
{
	for(uint8_t i=0; i < 16; i++){
		enckey[i] = key[i];
	}
}

void aes_indep_enc(uint8_t * pt)
{
	uint8_t * result = aes(pt, enckey);
	for(uint8_t i=0; i < 16; i++){
		pt[i] = result[i];
	}
}

#else

#error "No Crypto Lib Defined?"

#endif


