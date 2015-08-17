/* shavs.h */
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
 * \file	shavs.h
 * \author  Daniel Otte
 * \date    2006-05-16
 * \license	GPLv3 or later
 *
 */

#ifndef SHAVS_H_
#define SHAVS_H_

#include <stdlib.h>
#include "hashfunction_descriptor.h"

extern hfdesc_t*  shavs_algo;
extern hfdesc_t** shavs_algolist;

void shavs_listalgos(void);
void shavs_setalgo(char* param);
void shavs_test1(void);
void shavs_test2(void);
void shavs_test3(void);


#endif /* SHAVS */
