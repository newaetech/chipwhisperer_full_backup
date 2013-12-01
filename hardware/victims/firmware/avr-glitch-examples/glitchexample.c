/*
    This file is part of the ChipWhisperer Example Targets, this example does some 
    stuff you can glitch.
    Copyright (C) 2013 Colin O'Flynn <coflynn@newae.com>

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

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>

#include "uart.h"

void glitch1(void)
{
    //Can monitor Port C, Pin 5 (top right pin on 28-DIP)
    DDRC |= 1<<5;
    PORTC = 1<<5;
    
    //Some fake variable
    volatile uint8_t a = 0;
    
    output_ch_0('A');
    
    //Should be an infinite loop
    while(a != 2){
    ;
    }
    
    //Several examples in case glitching skips a few instructions
    PORTC = 0;
    PORTC = 0;

    output_ch_0('1');
    output_ch_0('2');
    output_ch_0('3');
    output_ch_0('4');

    PORTC = 0;
    PORTC = 0;
    PORTC = 0;
    PORTC = 0;
    PORTC = 0;

    //Several loops in order to try and prevent restarting
    while(1){
    ;
    }
    while(1){
    ;
    }
    while(1){
    ;
    }
    while(1){
    ;
    }
    while(1){
    ;
    }    
}

void glitch2()
{
    char c;
    output_ch_0('B');
    c = input_ch_0();
    
    if (c == 'q'){
        output_ch_0('1');
    } else {
        output_ch_0('2');
    }
}

void glitch3()
{
    char inp[16];
    char c;
    int cnt = 0;
    output_ch_0('C');

    c = 'A';
    while((c != '\n') & (cnt < 16)){
        c = input_ch_0();
        inp[cnt] = c;
        cnt++;
    }
    
    char passwd[] = "touch";
    char passok = 1;
    
    //Simple test - doesn't check for too-long password!
    for(cnt = 0; cnt < 5; cnt++){
        if (inp[cnt] != passwd[cnt]){
            passok = 0;
        }
    }
    
    if (!passok){
        output_ch_0('F');
        output_ch_0('O');
        output_ch_0('f');
        output_ch_0('f');
        output_ch_0('\n');
    } else {
        output_ch_0('W');
        output_ch_0('e');
        output_ch_0('l');
        output_ch_0('c');
        output_ch_0('o');
        output_ch_0('m');
        output_ch_0('e');
        output_ch_0('\n');
    }
}


int main
	(
	void
	)
	{
	init_uart0();
	
	/* For 2 MHz crystal use this hack */
	//BAUD0L_REG = 12;

 	/* Uncomment this to get a HELLO message for debug */	
	output_ch_0('h');
	output_ch_0('e');
	output_ch_0('l');
	output_ch_0('l');
	output_ch_0('o');
	output_ch_0('\n');
	
		
    glitch1();
        
	return 1;
	}
	
	