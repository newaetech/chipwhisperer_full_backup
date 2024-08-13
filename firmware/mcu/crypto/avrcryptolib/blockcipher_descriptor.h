/* blockcipher_descriptor.h */
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
/**
 * \file		blockcipher_descriptor.h
 * \author		Daniel Otte 
 * \date		2009-02-04
 * 
 * \license	    GPLv3 or later
 * 
 */

#ifndef BLOCKCIPHER_DESCRIPTOR_H_
#define BLOCKCIPHER_DESCRIPTOR_H_
#include <stdint.h>
#include <avr/pgmspace.h>


#ifndef VOID_FPT
#define VOID_FPT
typedef void(*void_fpt)(void);
#endif

typedef void(*bc_init1_fpt)(void*, void*);
typedef void(*bc_init2_fpt)(void*, uint16_t,void*);
typedef void(*bc_enc1_fpt)(void*, void*);
typedef void(*bc_enc2_fpt)(void*, void*, void*);
typedef void(*bc_dec1_fpt)(void*, void*);
typedef void(*bc_dec2_fpt)(void*, void*, void*);
typedef void(*bc_free_fpt)(void*);

typedef union{
	void_fpt  initvoid;
	bc_init1_fpt init1;
	bc_init2_fpt init2;
} bc_init_fpt;

typedef union{
	void_fpt  encvoid;
	bc_enc1_fpt enc1;
	bc_enc2_fpt enc2;
} bc_enc_fpt;

typedef union{
	void_fpt  decvoid;
	bc_dec1_fpt dec1;
	bc_dec2_fpt dec2;
} bc_dec_fpt;

#define BC_INIT_TYPE   0x01
#define BC_INIT_TYPE_1 0x00 /* for fix keylength */
#define BC_INIT_TYPE_2 0x01 /* keylength is passed as second parameter */

#define BC_ENC_TYPE    0x02
#define BC_ENC_TYPE_1  0x00
#define BC_ENC_TYPE_2  0x02
#
#define BC_DEC_TYPE    0x04
#define BC_DEC_TYPE_1  0x00
#define BC_DEC_TYPE_2  0x04

#define BCDESC_TYPE_BLOCKCIPHER 0x01

typedef struct {
	uint8_t  type; /* 1==blockcipher */
	uint8_t  flags;
	PGM_P    name;
	uint16_t ctxsize_B;
	uint16_t blocksize_b;
	bc_init_fpt init;
	bc_enc_fpt  enc;
	bc_dec_fpt  dec;
	bc_free_fpt free;
	PGM_VOID_P valid_keysize_desc;
} bcdesc_t; /* blockcipher descriptor type */

typedef struct{
	bcdesc_t* desc_ptr;
	uint16_t  keysize;
	void*     ctx;
} bcgen_ctx_t;

#endif /* BLOCKCIPHER_DESCRIPTOR_H_ */

