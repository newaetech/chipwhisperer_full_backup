/*
 Copyright (c) 2015 NewAE Technology Inc. All rights reserved.

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

#ifndef SCARD_USB_H_
#define SCARD_USB_H_

#include <asf.h>

/** Maximum ATR uc_size in bytes. */
#define MAX_ATR_SIZE            55

extern uint8_t scard_atr[MAX_ATR_SIZE];
extern uint8_t scard_atr_size;
extern uint8_t scard_protocol;

bool ctrl_scarddata_req(void);
bool ctrl_scardconfig_req(void);
bool ctrl_scardaux_req(void);
void ctrl_scarddata_cb(void);
void ctrl_scardconfig_cb(void);
void ctrl_scardaux_cb(void);
void scard_reset_init(void);


#endif /* SCARD_USB_H_ */