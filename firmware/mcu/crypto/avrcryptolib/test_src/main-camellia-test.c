/* main-camellia-test.c */
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
/*
 * camellia test-suit
 * 
 */

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "camellia.h"
#include "performance_test.h"
#include "cli.h"
#include "bcal_camellia128.h"
#include "bcal-performance.h"
#include "bcal-nessie.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

char* algo_name = "Camellia";

const bcdesc_t* const algolist[] PROGMEM = {
	(bcdesc_t*)&camellia128_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_camellia(void){
	bcal_nessie_multiple(algolist);
}

/*
 * P No.001 : 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 */

const uint8_t test_keys[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
	0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,
	0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
	0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
	0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE, 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01,
	0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01, 0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE
};

void hexdump128(void* data){
	uint8_t i;
	for(i=0; i<16; ++i){
		cli_hexdump(data, 1);
		cli_putc(' ');
		data = (uint8_t*)data +1;
	}
}

void testrun_camellia128(void){
	uint8_t data[16];
	uint8_t data2[16];
	uint8_t key[16];
	char str[4];
	uint8_t i,j;
	str[3]= '\0';
	for(j=0; j<10; ++j){
		str[0] = ('0'+(j+1)/100);
		str[1] = ('0'+((j+1)/10)%10);
		str[2] = ('0'+(j+1)%10);
		memcpy_P(key, test_keys+j*16, 16);
		cli_putstr_P(PSTR("\r\nK No."));
		cli_putstr(str);
		cli_putstr_P(PSTR(" : "));
		hexdump128(key);
		camellia128_ctx_t ctx;
		camellia128_init(key, &ctx);
		for(i=0; i<128; ++i){
			memset(data, 0x00, 16);
			data[i/8] = 0x80>>i%8;
			memcpy(data2, data, 16);
			str[0] = ('0'+(i+1)/100);
			str[1] = ('0'+((i+1)/10)%10);
			str[2] = ('0'+(i+1)%10);
			cli_putstr_P(PSTR("\r\nP No."));
			cli_putstr(str);
			cli_putstr_P(PSTR(" : "));
			hexdump128(data);
			camellia128_enc(data, &ctx);
			cli_putstr_P(PSTR("\r\nC No."));
			cli_putstr(str);
			cli_putstr_P(PSTR(" : "));
			hexdump128(data);
			camellia128_dec(data, &ctx);
			if(memcmp(data, data2, 16)){
				cli_putstr_P(PSTR("\r\n DECRYPTION ERROR !!!"));
			}
		}
	}
}

void test_performance_camellia(void){
	bcal_performance_multiple(algolist);
}


/*****************************************************************************
 *  self tests																 *
 *****************************************************************************/
/*
128-bit key
key         01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
plaintext   01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
ciphertext  67 67 31 38 54 96 69 73 08 57 06 56 48 ea be 43
*/
void testrun_camellia(void){

  uint8_t data[16] = { 0x01, 0x23, 0x45, 0x67, 
                       0x89, 0xab, 0xcd, 0xef, 
                       0xfe, 0xdc, 0xba, 0x98, 
                       0x76, 0x54, 0x32, 0x10 };

  uint8_t key[16] = { 0x01, 0x23, 0x45, 0x67, 
                       0x89, 0xab, 0xcd, 0xef, 
                       0xfe, 0xdc, 0xba, 0x98, 
                       0x76, 0x54, 0x32, 0x10 };


  camellia128_ctx_t ctx;
  camellia128_init(key, &ctx);
  cli_putstr_P(PSTR("\r\n key:        "));
  cli_hexdump(data, 16);
  cli_putstr_P(PSTR("\r\n plaintext:  "));
  cli_hexdump(data, 16);
  camellia128_enc(data, &ctx);
  cli_putstr_P(PSTR("\r\n ciphertext: "));
  cli_hexdump(data, 16);
  camellia128_dec(data, &ctx);
  cli_putstr_P(PSTR("\r\n decrypted:  "));
  cli_hexdump(data, 16);

}


/*****************************************************************************
 * main                                                                      *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char test128_str[]     PROGMEM = "test128";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_camellia },
	{ test_str,        NULL, testrun_camellia},
	{ test128_str,     NULL, testrun_camellia128},
	{ performance_str, NULL, test_performance_camellia},
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main (void){
	DEBUG_INIT();
	
	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;	 	
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}

