/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2015 NewAE Technology Inc.

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

#include <stdio.h>
#include "hal.h"


void uart_puts(char * s){
    while(*s){
        putch(*(s++));
    }
}

void glitch_infinite(void)
{
    char str[64];
    unsigned int k = 0;
    //Declared volatile to avoid optimizing away loop.
    //This also adds lots of SRAM access
    volatile uint16_t i, j;
    volatile uint32_t cnt;
    while(1){
        cnt = 0;
        trigger_high();
        trigger_low();
        for(i=0; i<200; i++){
            for(j=0; j<200; j++){
                cnt++;
            }
        }
        sprintf(str, "%lu %d %d %d\n", cnt, i, j, k++);
        uart_puts(str);
    }
}

void glitch1(void)
{
    led_ok(1);
    led_error(0);
    
    //Some fake variable
    volatile uint8_t a = 0;
    
    putch('A');
    
    //External trigger logic
    trigger_high();
    trigger_low();
    
    //Should be an infinite loop
    while(a != 2){
    ;
    }    
    
    led_error(1);
    led_error(1);
    led_error(1);
    led_error(1);
    led_error(1);
    led_error(1);
    led_error(1);
    led_error(1);
    led_error(1);
    
    uart_puts("1234");
    
    led_error(1);
    led_error(1);
    led_error(1);
    led_error(1);
    led_error(1);
    led_error(1);
    led_error(1);
    led_error(1);

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


void glitch2(void)
{
    char c;  
    
    putch('B');
       
    c = getch();
    
    trigger_high();
    trigger_low();
    
    if (c != 'q'){
        putch('1');
    } else {
        putch('2');
    }
    putch('\n');
    putch('\n');
    putch('\n');
    putch('\n');
}


void glitch3(void)
{
    char inp[16];
    char c = 'A';
    unsigned char cnt = 0;
    uart_puts("Password:");
    
    while((c != '\n') & (cnt < 16)){
        c = getch();
        inp[cnt] = c;
        cnt++;
    }
    
    char passwd[] = "touch";
    char passok = 1;
    
    trigger_high();
    trigger_low();
    
    //Simple test - doesn't check for too-long password!
    for(cnt = 0; cnt < 5; cnt++){
        if (inp[cnt] != passwd[cnt]){
            passok = 0;
        }
    }
    
    if (!passok){
        uart_puts("Denied\n");
    } else {
        uart_puts("Welcome\n");
    }
}

int main(void){

    platform_init();
	init_uart();	
	trigger_setup();

 	/* Uncomment this to get a HELLO message for debug */
	putch('h');
	putch('e');
	putch('l');
	putch('l');
	putch('o');
	putch('\n');
    //_delay_ms(20);
		
        
    while(1){
        glitch_infinite();
    }
        
	return 1;
}