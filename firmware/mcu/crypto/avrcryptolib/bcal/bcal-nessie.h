/* bcal-nessie.h */
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
 * \file    bcal-nessie.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2010-12-19
 * \license GPLv3 or later
 *
 */

#ifndef BCALNESSIE_H_
#define BCALNESSIE_H_

#include "blockcipher_descriptor.h"

void bcal_nessie(const bcdesc_t* bcd);
void bcal_nessie_multiple(const bcdesc_t* const* bcd_list);


#endif /* BCALNESSIE_H_ */
