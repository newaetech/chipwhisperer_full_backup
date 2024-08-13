/* sha2_large_common.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2011 Daniel Otte (daniel.otte@rub.de)

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

#ifndef SHA2_LARGE_COMMON_H_
#define SHA2_LARGE_COMMON_H_

typedef struct {
	uint64_t h[8];
	uint32_t length;
} sha2_large_common_ctx_t;


void sha2_large_common_nextBlock(sha2_large_common_ctx_t* ctx, const void* block);
void sha2_large_common_lastBlock(sha2_large_common_ctx_t* ctx, const void* block, uint16_t length_b);


#endif /* SHA2_LARGE_COMMON_H_ */
