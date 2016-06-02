/*
 Copyright (c) 2015, NewAE Technology Inc
 All rights reserved.

 Authors: Colin O'Flynn

 Find this and more at newae.com - this file is part of the chipwhisperer
 project, http://www.assembla.com/spaces/chipwhisperer

    This file is part of chipwhisperer.

    chipwhisperer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    chipwhisperer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _AES_MODEL_H
#define _AES_MODEL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TARGET_SBOXOUT_HW 1
#define TARGET_INVSBOX_LASTROUND_HD 2
#define TARGET_SBOXINOUT_HD 3
#define TARGET_SBOXIN_HD 4
#define TARGET_SBOXOUT_HD 5

typedef struct {
  unsigned int  bnum;           // Byte number to attack
  uint8_t *     roundkeys;      // Table of all round keys if known or recovered
  unsigned int  rtarget;        // Target round to attack
  unsigned int  leakagemode;    // Leakage Mode + Target
  void *        privatedata;
  unsigned int  privatedatasize;
} model_setup_t;

extern const int hamming_weight[];
extern const unsigned char sbox[256];
double aes_model(uint8_t guess, uint8_t * datain, uint8_t * dataout, void * modeldata);

#ifdef __cplusplus
}
#endif

#endif //_CPA_PROGRESSIVE_H

