/* mqq160-sign.h */
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

#ifndef MQQ160SIGN_H_
#define MQQ160SIGN_H_

typedef struct{
	const uint8_t *a;
	const uint8_t *cc1;
	const uint8_t *cc2;
	const uint8_t *rp1;
	const uint8_t *rp5;
} mqq160_sign_key_t;

void mqq160_sign(void* dest, const void* hash, const mqq160_sign_key_t* key);

#endif /* MQQ160SIGN_H_ */
