/* debug.c */
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
/***************************
*
*
*
****************************/
#include "config.h"

#if DEBUG_METHOD == uart
 #include "uart_i.h"
#else
  #error "Your DEBUG methode is not suported!"
#endif

#ifdef DEBUG_METHOD
 void debug_init(void){
 #if DEBUG_METHOD==uart
  uart0_init();
 #else
  #error "Your DEBUG methode is not suported!"
 #endif
 }

 void debug_char(char c){
	static char initialised = 0;
	if (!initialised){
		uart0_init();
		initialised=1;
	}
	uart0_putc(c);
 }

 void debug_str(char* s){
 	while (*s)
 		debug_char(*s++);
 }



 void debug_byte(char b){
 	char table[] = "0123456789ABCDEF";
 	debug_char(table[(b>>4) & 0xf]);
 	debug_char(table[b&0xf]);
 }

#endif //DEBUG

