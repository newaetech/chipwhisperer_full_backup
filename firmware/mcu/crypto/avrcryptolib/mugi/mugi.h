/* mugi.h */
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
 * \file	mugi.h
 * \author	Daniel Otte 
 * \date	2009-02-15
 * \brief 	implementation of the MUGI key stream generator
 * \license	GPLv3 or later
 */
 
#ifndef MUGI_H_
#define MUGI_H_ 

#include <stdint.h>

typedef struct{
	uint64_t a[3];
	uint64_t b[16];
}mugi_ctx_t; /* 152 bytes in total */

void mugi_init(const void* key, const void* iv, mugi_ctx_t* ctx);
uint64_t mugi_gen(mugi_ctx_t* ctx);


#endif /* MUGI_H_ */
