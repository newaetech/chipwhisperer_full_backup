/* streamcipher_descriptor.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2011  Daniel Otte (daniel.otte@rub.de)

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
 * \file		stremcipher_descriptor.h
 * \author		Daniel Otte 
 * \date		2011-01-15
 * 
 * \license	    GPLv3 or later
 * 
 */

#ifndef STREAMCIPHER_DESCRIPTOR_H_
#define STREAMCIPHER_DESCRIPTOR_H_
#include <stdint.h>
#include <avr/pgmspace.h>


#ifndef VOID_FPT
#define VOID_FPT
typedef void(*void_fpt)(void);
#endif

typedef void(*sc_init1_fpt)(const void*, void*); /* key of fixed length, no IV*/
typedef void(*sc_init2_fpt)(const void*, const void*, void*); /* key and IV of fixed length */
typedef void(*sc_init3_fpt)(const void*, uint16_t,void*); /* key of variable length, no IV */
typedef void(*sc_init4_fpt)(const void*, uint16_t,const void*, void*); /* key of variable length, IV of fixed length */
typedef void(*sc_init5_fpt)(const void*, uint16_t,const void*, uint16_t, void*); /* key and IV of variable length */

typedef uint8_t(*sc_gen1_fpt)(void*); /* return keystream data */
typedef void(*sc_gen2_fpt)(void*, void*); /* write keystream data to buffer */

typedef uint8_t(*sc_genra1_fpt)(void*, uint64_t); /* return keystream data */
typedef void(*sc_genra2_fpt)(void*, void*, uint64_t); /* write keystream data to buffer */


typedef void(*sc_free_fpt)(void*);

typedef union{
	void_fpt  initvoid;
	sc_init1_fpt init1;
	sc_init2_fpt init2;
	sc_init3_fpt init3;
	sc_init4_fpt init4;
	sc_init5_fpt init5;
} sc_init_fpt;

typedef union{
	void_fpt  genvoid;
	sc_gen1_fpt gen1;
	sc_gen2_fpt gen2;
} sc_gen_fpt;

typedef union{
	void_fpt  genravoid;
	sc_genra1_fpt genra1;
	sc_genra2_fpt genra2;
} sc_genra_fpt;

#define SC_INIT_TYPE   0x07
#define SC_INIT_TYPE_1 0x00 /* key of fixed length, no IV*/
#define SC_INIT_TYPE_2 0x02 /* key and IV of fixed length */
#define SC_INIT_TYPE_3 0x03 /* key of variable length, no IV */
#define SC_INIT_TYPE_4 0x04 /* key of variable length, IV of fixed length */
#define SC_INIT_TYPE_5 0x05 /* key and IV of variable length */

#define SC_GEN_TYPE    0x08
#define SC_GEN_TYPE_1  0x00 /* return data stream byte */
#define SC_GEN_TYPE_2  0x08 /* write data stream block into buffer */

#define SCDESC_TYPE_STREAMCIPHER 0x03

typedef struct {
	uint8_t  type; /* 3==streamcipher */
	uint8_t  flags;
	PGM_P    name;
	uint16_t ctxsize_B;
	uint16_t gensize_b;
	sc_init_fpt init;
	sc_gen_fpt  gen;
	sc_genra_fpt  genra;
	sc_free_fpt free;
	PGM_VOID_P valid_keysize_desc;
	PGM_VOID_P valid_ivsize_desc;
} scdesc_t; /* streamcipher descriptor type */

typedef struct{
	const scdesc_t* desc_ptr;
	uint16_t        keysize;
	uint16_t        ivsize;
	uint16_t        index;
	uint8_t*        buffer;
	void*           ctx;
} scgen_ctx_t;

#endif /* STREAMCIPHER_DESCRIPTOR_H_ */

