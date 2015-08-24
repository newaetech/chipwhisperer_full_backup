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

#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define IDLE 0
#define KEY 1
#define PLAIN 2

#define BUFLEN 64

uint8_t memory[BUFLEN];
uint8_t tmp[BUFLEN];
char asciibuf[BUFLEN];
uint8_t pt[16];

/* Enable printf() / scanf() use for application */
static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);
FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE mystdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

static int uart_putchar(char c, FILE *stream)
{
	putch(c);
	return 0;
}

static int uart_getchar(FILE *stream)
{
	return getch();
}

int main(void)
	{
    platform_init();
	init_uart();	
	trigger_setup();
    stdout = &mystdout;
	stdin = &mystdin;
		
    char passwd[32];	 
    char correct_passwd[] = "h0px3";
     
	while(1){
	
        printf("*****Safe-o-matic 3000 Booting...\n");
        //Print some fancy-sounding stuff so that attackers
        //will get scared and leave us alone
        printf("Aligning bits........[DONE]\n");
        _delay_ms(200);
        printf("Checking Cesium RNG..[DONE]\n");
        _delay_ms(200);
        printf("Masquerading flash...[DONE]\n");
        _delay_ms(200);
        printf("Decrypting database..[DONE]\n");
        _delay_ms(200);
        printf("\n\n");
              
        //Give them one last warning
        printf("WARNING: UNAUTHORIZED ACCESS WILL BE PUNISHED\n");
        
        trigger_low();
        
        //Get password
        printf("Please enter password to continue: ");
    	scanf("%s", passwd);
        
        uint8_t passbad = 0;
        
        trigger_high();
        
        for(uint8_t i = 0; i < sizeof(correct_passwd); i++){
            if (correct_passwd[i] != passwd[i]){
                passbad = 1;
                break;
            }
        }
		
        if (passbad){
            //Stop them fancy timing attacks
            int wait = rand();
            for(volatile int i = 0; i < wait; i++){
                ;
            }
            _delay_ms(500);
            printf("PASSWORD FAIL\n");        
            led_error(1);
        } else {
            printf("Access granted, Welcome!\n");
            led_ok(1);
        }

        //All done;
        while(1);
	}
		
	return 1;
	}
	
	