/* main-test-common.c */
/*
    This file is part of the ARM-Crypto-Lib.
    Copyright (C) 2006-2011 Daniel Otte (daniel.otte@rub.de)

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

#include "main-test-common.h"

void main_setup(void){
	DEBUG_INIT();
	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;
}

void welcome_msg(const char* algoname){
	cli_putstr_P(PSTR("\r\n\r\nAVR-Crypto-Lib VS ("));
	cli_putstr(algoname);
	cli_putstr_P(PSTR("; "));
	cli_putstr(__DATE__);
	cli_putc(' ');
	cli_putstr(__TIME__);
	cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
}
