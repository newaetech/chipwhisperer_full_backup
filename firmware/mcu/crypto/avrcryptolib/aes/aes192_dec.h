/* aes192_dec.h */
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
 * \file     aes192_dec.h
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte
 * \date     2008-12-31
 * \license  GPLv3 or later
 * \ingroup  AES
 */

#ifndef AES192_DEC_H_
#define AES192_DEC_H_

#include "aes_types.h"
#include "aes_dec.h"

/**
 * \brief decrypt with 192 bit key.
 *
 * This function decrypts one block with the AES algorithm under control of
 * a keyschedule produced from a 192 bit key.
 * \param buffer pointer to the block to decrypt
 * \param ctx    pointer to the key schedule
 */
void aes192_dec(void* buffer, aes192_ctx_t* ctx);



#endif /* AES192_DEC_H_ */
