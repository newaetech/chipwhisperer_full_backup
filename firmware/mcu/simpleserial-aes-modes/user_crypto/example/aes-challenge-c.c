/*
    This file is part of the AESExplorer Example Targets
    Copyright (C) 2012 Colin O'Flynn <coflynn@newae.com>

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

#include "aes-challenge.h"

/* WARNING: Be VERY CAREFUL includinf a c-file like this. You are likely to get variable name collisions,
            so suggest to avoid using any generic variable names within this file */
#include "aes.c"

uint8_t _stored_key[16];
uint8_t * _stored_ct;

void aes_indep_init(void)
{
    ;
}

void aes_indep_key(uint8_t * key)
{
    for (uint8_t i = 0; i < 16; i++){
        _stored_key[i] = key[i];
    }
}

void aes_indep_enc(uint8_t * pt)
{
	_stored_ct = aes(pt, _stored_key);
    
    for (uint8_t i = 0; i < 16; i++){
        pt[i] = _stored_ct[i];
    }
}
