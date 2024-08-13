/* cmacvs.h */
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
 * \file	cmacvs.h
 * \author  Daniel Otte
 * \date    2010-02-02
 * \license	GPLv3 or later
 *
 */

#ifndef CMACVS_H_
#define CMACVS_H_

#include <stdlib.h>
#include "blockcipher_descriptor.h"

extern bcdesc_t*  cmacvs_algo;
extern bcdesc_t** cmacvs_algolist;

void cmacvs_listalgos(void);
void cmacvs_setalgo(char* param);
void cmacvs_test1(void);
void cmacvs_test2(void);


#endif /* CMACVS_H_ */
