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
#include "hal.h"

#if HW_CRYPTO

void aes_indep_init(void)
{
    HW_AES128_Init();
	
}

void aes_indep_key(uint8_t * key)
{
    HW_AES128_LoadKey(key);
}

void aes_indep_enc(uint8_t * pt)
{
    HW_AES128_Enc(pt);
}

#elif defined(AVRCRYPTOLIB)
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

#elif defined(DPAV4)

#include "aes.h"
#include "aes_enc.h"

/*  This is the AES RSM 256 encryption function that call the generic AES RSM encryption core*/
void aes256_enc(uint8_t* j, void* buffer, aes256_ctx_t* ctx,uint8_t rng){
	aes_encrypt_core(j,buffer, (aes_genctx_t*)ctx, 14,(uint8_t)rng);
}

aes256_ctx_t ctx;

void aes_indep_init(void)
{
    ;
}

void aes_indep_key(uint8_t * key)
{
	aes256_init(key, &ctx);
}

void aes_indep_enc(uint8_t * pt)
{
	static uint8_t j[0];

	//Encryption with trigger enabled
	aes256_enc(j, pt, &ctx, 1);
}


#else

#error "No Crypto Lib Defined?"

#endif


