/* main-hmac-md5-test.c */
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
 * HMAC-MD5 test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "md5.h"
#include "hmac-md5.h"
/*
#include "base64_enc.h"
#include "base64_dec.h"
*/
#include "nessie_mac_test.h"

#include <stdint.h>
#include <string.h>
#include "cli.h"

char* algo_name = "HMAC-MD5";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void testrun_nessie_hmacmd5(void){
	nessie_mac_ctx.macsize_b   = HMAC_MD5_BITS;
	nessie_mac_ctx.keysize_b   = HMAC_MD5_BLOCK_BITS;
	nessie_mac_ctx.blocksize_B = HMAC_MD5_BLOCK_BYTES;
	nessie_mac_ctx.ctx_size_B  = sizeof(hmac_md5_ctx_t);
	nessie_mac_ctx.name = algo_name;
	nessie_mac_ctx.mac_init = (nessie_mac_init_fpt)hmac_md5_init;
	nessie_mac_ctx.mac_next = (nessie_mac_next_fpt)hmac_md5_nextBlock;
	nessie_mac_ctx.mac_last = (nessie_mac_last_fpt)hmac_md5_lastBlock;
	nessie_mac_ctx.mac_conv = (nessie_mac_conv_fpt)hmac_md5_final;
	
	nessie_mac_run();
}

void testrun_test_hmacmd5(void){
	uint8_t hmac[16];
	uint8_t keys[][16] = {
		{ 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
		  0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b },
		{ 'J', 'e', 'f', 'e', },
		{ 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
		  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA }
	};
	uint8_t buffer[50];
	cli_putstr_P(PSTR("\r\n hmac (1): "));
	hmac_md5(hmac, keys[0], 128, "Hi There", 64);
	cli_hexdump(hmac, 16);
	
	cli_putstr_P(PSTR("\r\n hmac (2): "));
	hmac_md5(hmac, keys[1], 4*8, "what do ya want for nothing?", 28*8);
	cli_hexdump(hmac, 16);
	
	cli_putstr_P(PSTR("\r\n hmac (3): "));
	memset(buffer, 0xDD, 50);
	hmac_md5(hmac, keys[2], 128, buffer, 50*8);
	cli_hexdump(hmac, 16);
	
}

void hmacmd5_interactive(void){
	char key[101];
	char msg[101];
	uint8_t hmac[HMAC_MD5_BYTES];
	uint8_t key_len, msg_len;
	cli_putstr_P(PSTR("\r\nHMAC-MD5:\r\nkey: "));
	cli_getsn(key, 100);
	key_len = strlen(key);
	cli_putstr_P(PSTR("\r\nmsg: "));
	cli_getsn(msg, 100);
	msg_len = strlen(msg);
	hmac_md5(hmac, key, key_len*8, msg, msg_len*8);
	cli_putstr_P(PSTR("\r\nhmac-md5: "));
	cli_hexdump(hmac, HMAC_MD5_BYTES);
}

void strhexdump(char* dest, void* src, uint16_t length){
	char table[] = { '0', '1', '2', '3', 
	                 '4', '5', '6', '7', 
					 '8', '9', 'a', 'b', 
					 'c', 'd', 'e', 'f' };
	while(length--){
		*dest++ = table[(*((uint8_t*)src))>>4];
		*dest++ = table[(*((uint8_t*)src))&0xf];
		src = (uint8_t*)src +1;
	}
}

/*
void cram_md5_interactive(void){
	char key[101];
	char msg_b64[101];
	char username[101];
	uint8_t msg[100];
	uint8_t hmac[HMAC_MD5_BYTES];
	int key_len, msg_len;
	int ul;
	cli_putstr_P(PSTR("\r\nCRAM-MD5:\r\nkey: "));
	cli_getsn(key, 100);
	key_len = strlen(key);
	cli_putstr_P(PSTR("\r\nusername: "));
	cli_getsn(username, 60);
	cli_putstr_P(PSTR("\r\nchallange: "));
	cli_getsn(msg_b64, 100);
	if((msg_len = base64_binlength(msg_b64, 1))==-1){
		cli_putstr_P(PSTR("\r\nERROR in base64 encoding !\r\n"));
		return;
	}
	base64dec(msg, msg_b64, 1);
	hmac_md5(hmac, key, key_len*8, msg, msg_len*8);
	ul=strlen(username);
	username[ul]=' ';
	strhexdump(username+ul+1, hmac, 128/8);
	base64enc(msg_b64, username, ul+1+128/8*2);
	cli_putstr_P(PSTR("\r\nresponse: "));
	cli_hexdump(hmac, HMAC_MD5_BYTES);
	cli_putstr_P(PSTR("\r\nresponse (b64): "));
	cli_putstr(msg_b64);	
}
*/


void md5_interactive(void){
	char msg[101];
	uint8_t hash[MD5_HASH_BYTES];
	uint8_t msg_len;
	cli_putstr_P(PSTR("\r\nmsg: "));
	cli_getsn(msg, 100);
	msg_len = strlen(msg);
	md5((void*)hash, msg, msg_len*8);
	cli_putstr_P(PSTR("\r\nmd5: "));
	cli_hexdump(hash, MD5_HASH_BYTES);
}


/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
/* const char performance_str[] PROGMEM = "performance"; */
const char echo_str[]        PROGMEM = "echo";
const char hmd5i_str[]       PROGMEM = "hmac-md5";
/* const char crammd5i_str[]    PROGMEM = "cram-md5"; */
const char md5i_str[]        PROGMEM = "md5";


const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_hmacmd5},
	{ test_str,        NULL, testrun_test_hmacmd5},
	{ hmd5i_str,       NULL, hmacmd5_interactive},
/*	{ crammd5i_str,    NULL, cram_md5_interactive},        */
	{ md5i_str,        NULL, md5_interactive},
/*	{ performance_str, NULL, testrun_performance_hmacmd5}, */
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
