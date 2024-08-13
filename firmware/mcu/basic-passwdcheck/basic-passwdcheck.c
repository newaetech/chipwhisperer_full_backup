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

static void delay_2_ms(void);


void my_puts(char *c)
{
  do {
    putch(*c);

  } while (*++c);
}

static void delay_2_ms()
{
  for (volatile unsigned int i=0; i < 0xfff; i++ ){
    ;
  }
}

void my_read(char *buf, int len)
{
  for(int i = 0; i < len; i++) {
    while (buf[i] = getch(), buf[i] == '\0');

    if (buf[i] == '\n') {
      buf[i] = '\0';
      return;
    }
  }
  buf[len - 1] = '\0';
}

int main(void)
  {
    platform_init();
  init_uart();
  trigger_setup();

    char passwd[32];
    char correct_passwd[] = "h0px3";

  while(1){

        my_puts("*****Safe-o-matic 3000 Booting...\n");
        //Print some fancy-sounding stuff so that attackers
        //will get scared and leave us alone
        my_puts("Aligning bits........[DONE]\n");
        delay_2_ms();
        my_puts("Checking Cesium RNG..[DONE]\n");
        delay_2_ms();
        my_puts("Masquerading flash...[DONE]\n");
        delay_2_ms();
        my_puts("Decrypting database..[DONE]\n");
        delay_2_ms();
        my_puts("\n\n");

        //Give them one last warning
        my_puts("WARNING: UNAUTHORIZED ACCESS WILL BE PUNISHED\n");

        trigger_low();

        //Get password
        my_puts("Please enter password to continue: ");
        my_read(passwd, 32);

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
             int wait = 1;
            for(volatile int i = 0; i < wait; i++){
                ;
            }
            delay_2_ms();
            delay_2_ms();
            my_puts("PASSWORD FAIL\n");
            led_error(1);
        } else {
            my_puts("Access granted, Welcome!\n");
            led_ok(1);
        }

        //All done;
        while(1);
  }

  return 1;
  }


