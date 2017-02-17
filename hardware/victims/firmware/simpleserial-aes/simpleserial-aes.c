/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2017 NewAE Technology Inc.

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

#include "aes-independant.h"
#include "hal.h"
#include "simpleserial.h"
#include <stdint.h>
#include <stdlib.h>




void get_key(uint8_t* k)
{
	aes_indep_key(k);
}

void get_pt(uint8_t* pt)
{
    trigger_high();
	aes_indep_enc(pt); /* encrypting the data block */
    trigger_low();
    simpleserial_put('r', 16, pt);
}

void reset(uint8_t* x)
{
    // Reset key here if needed
}

int main(void)
{
	uint8_t tmp[KEY_LENGTH] = {DEFAULT_KEY};

    platform_init();
    init_uart();
    trigger_setup();

	aes_indep_init();
	aes_indep_key(tmp);

    /* Uncomment this to get a HELLO message for debug */
    /*
    putch('h');
    putch('e');
    putch('l');
    putch('l');
    putch('o');
    putch('\n');
    */

    simpleserial_addcmd('k', 16, get_key);
    simpleserial_addcmd('p', 16, get_pt);
    simpleserial_addcmd('x', 0, reset);
    while(1)
        simpleserial_get();
}
