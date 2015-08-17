/* mqq160-sign_testkey.h */
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

#ifndef MQQ160SIGN_TESTKEY_H_
#define MQQ160SIGN_TESTKEY_H_

#include <stdint.h>

#define MQQ160SIGN_A_SIZE    80
#define MQQ160SIGN_RP1_SIZE 160
#define MQQ160SIGN_RP5_SIZE 160
#define MQQ160SIGN_CC1_SIZE   9
#define MQQ160SIGN_CC2_SIZE   9
#define MQQ160SIGN_KEY_SIZE (9+9+160+160+80)

/*
This is the private key of MQQ defined by one
quadratic quasigroup of order 2^8 given as 81 uint8_ts
and one nonsingular matrix SInv given as two arrays
RP1[] and RP5[] of 160 uint8_ts.
*/
extern const mqq160_sign_key_t testkey_P;

void mqq_load_pgm_key(void* buffer, mqq160_sign_key_t* key, const mqq160_sign_key_t* key_P);

#endif /* MQQ160SIGN_TESTKEY_H_ */
