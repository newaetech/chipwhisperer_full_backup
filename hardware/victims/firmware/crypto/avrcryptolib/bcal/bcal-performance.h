/* bcal-performance.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)

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
 * \file    bcal-performance.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2010-02-16
 * \license GPLv3 or later
 *
 */

#ifndef BCAL_PERFORMANCE_H_
#define BCAL_PERFORMANCE_H_

#include "blockcipher_descriptor.h"

void bcal_performance(const bcdesc_t* hd);
void bcal_performance_multiple(const bcdesc_t* const* hd_list);


#endif /* BCAL_PERFORMANCE_H_ */
