/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2019 NewAE Technology Inc.

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

#include "hal.h"
#include <stdint.h>
#include <stdlib.h>

#include "simpleserial.h"

uint8_t process_secret_key(uint8_t* key, uint8_t len);
uint8_t process_input(uint8_t* pt, uint8_t len);

uint8_t reset(uint8_t* x, uint8_t len)
{
	// Reset key here if needed
	return 0x00;
}

int main(void)
{
    platform_init();
	init_uart();
	trigger_setup();

	simpleserial_init();
	simpleserial_addcmd('k', SECRET_KEY_LEN, process_secret_key);
	simpleserial_addcmd('p', INPUT_LEN, process_input);
	while(1)
		simpleserial_get();
}
