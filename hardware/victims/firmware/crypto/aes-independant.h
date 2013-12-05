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

#ifndef AES_INDEPENDANT_
#define AES_INDEPENDANT_

#include <stdint.h>

#ifdef DPAV4
#define KEY_LENGTH 32
#define DEFAULT_KEY 0x6c,0xec,0xc6,0x7f,0x28,0x7d,0x08,0x3d, \
		0xeb,0x87,0x66,0xf0,0x73,0x8b,0x36,0xcf, \
		0x16,0x4e,0xd9,0xb2,0x46,0x95,0x10,0x90, \
		0x86,0x9d,0x08,0x28,0x5d,0x2e,0x19,0x3b  
#else
#define KEY_LENGTH 16
#define DEFAULT_KEY 0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c
#endif

void aes_indep_init(void);
void aes_indep_key(uint8_t * key);
void aes_indep_enc(uint8_t * pt);

#endif
