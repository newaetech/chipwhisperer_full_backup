/* dump.c */
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
 * \file     dump.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-02-04
 * \license  GPLv3 or later
 * 
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>					
#include <ctype.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "cli.h" 
#include "string-extras.h" 
 
#define DUMP_WIDTH 16

void pgm_read_block(void* buffer, uint32_t addr, uint8_t length);
void ee_read_block(void* buffer, uint32_t addr, uint8_t length);
void ram_read_block(void* buffer, uint32_t addr, uint8_t length);

const char flash_trigger[]  PROGMEM = "fF";
const char ram_trigger[]    PROGMEM = "sSrRmM";
const char eeprom_trigger[] PROGMEM = "eE";

const char flash_desc[]  PROGMEM = "flash";
const char ram_desc[]    PROGMEM = "ram";
const char eeprom_desc[] PROGMEM = "eeprom";

typedef const struct {
	PGM_P trigger;
	PGM_P desc;
	void (*fpt)(void*, uint32_t, uint8_t);
} memtype_desc_t;

memtype_desc_t memtype_desc[] PROGMEM = {
	{ flash_trigger,   flash_desc,      pgm_read_block },
	{ eeprom_trigger,  eeprom_desc,     ee_read_block  },
	{ ram_trigger,     ram_desc   ,     ram_read_block },
	{ NULL,            NULL,            NULL        }
};

void dump(char* s);
