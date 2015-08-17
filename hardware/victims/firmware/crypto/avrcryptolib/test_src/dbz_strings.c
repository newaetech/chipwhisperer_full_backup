/* dbz_strings.c */
/*
 *   This file is part of AnonAccess, an access system which can be used
 *    to open door or doing other things with an anonymity featured
 *    account managment.
 *   Copyright (C) 2006, 2007, 2008, 2009  Daniel Otte (daniel.otte@rub.de)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * */

#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
 
/******************************************************************************/

uint8_t dbz_strcount(const char* str){
	uint8_t ret=1;
	if(*str=='\0' && *(str+1)=='\0')
			return 0;
	for(;;){
		while(*str++)
			;
		if(*str=='\0')
			return ret;
		++ret;
	}	
}

/******************************************************************************/

void dbz_splitup(char* dbzstr, char** strings){
	if(*dbzstr=='\0' && *(dbzstr+1)=='\0')
		return;
	*strings++ = dbzstr;
	for(;;){	
		while(*dbzstr++)
			;
		if(*dbzstr=='\0')
			return;
		*strings++ = dbzstr;
	}
}

/******************************************************************************/

uint8_t dbz_strcount_P(PGM_P str){
	uint8_t ret=1;
	if(pgm_read_byte(str)=='\0' && pgm_read_byte(str+1)=='\0')
			return 0;
	for(;;){
		while(pgm_read_byte(str++))
			;
		if(pgm_read_byte(str)=='\0')
			return ret;
		++ret;
	}	
}

/******************************************************************************/

void dbz_splitup_P(PGM_P dbzstr, PGM_P* strings){
	if(pgm_read_byte(dbzstr)=='\0' && pgm_read_byte(dbzstr+1)=='\0')
		return;
	*strings++ = dbzstr;
	for(;;){	
		while(pgm_read_byte(dbzstr++))
			;
		if(pgm_read_byte(dbzstr)=='\0')
			return;
		*strings++ = dbzstr;
	}
}

/******************************************************************************/

