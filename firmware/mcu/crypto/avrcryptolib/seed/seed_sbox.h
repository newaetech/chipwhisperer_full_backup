/* seed_sbox.h */
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
 * \file	seed_sbox.h
 * \author	Daniel Otte 
 * \date	2007-06-1
 * \brief 	sboxes and constants for seed
 * \par License	
 * GPL
 * 
 */
 
#ifndef SEED_SBOX_H_
#define SEED_SBOX_H_

#include <stdint.h>
#include <avr/pgmspace.h>

extern const uint8_t seed_sbox1[256];

extern const uint8_t seed_sbox2[256];

/* key constants */
extern const uint32_t seed_kc[16];

#endif /*SEED_SBOX_H_*/
