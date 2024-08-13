/*
 * Mickey128 test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"
#include "cli.h"

#include "mickey128.h"
#include "scal_mickey128.h"
#include "scal-basic.h"
#include "scal-nessie.h"

#include <stdint.h>
#include <string.h>

char* algo_name = "Mickey128";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_mickey128(void){
	scal_nessie_set_estream(1);
	scal_nessie_run(&mickey128_desc);
}

void testrun_ref_mickey128(void){
	uint8_t key[4][16] ={
		                  { 0x12, 0x34, 0x56, 0x78, 
	                        0x9a, 0xbc, 0xde, 0xf0, 
	                        0x01, 0x23, 0x45, 0x67, 
	                        0x89, 0xab, 0xcd, 0xef},
	                      { 0xf1, 0x1a, 0x56, 0x27, 
	                        0xce, 0x43, 0xb6, 0x1f, 
	                        0x89, 0x12, 0x29, 0x94, 
	                        0x86, 0x09, 0x44, 0x86},
	                      { 0x3b, 0x80, 0xfc, 0x8c, 
	                        0x47, 0x5f, 0xc2, 0x70, 
	                        0xfa, 0x26, 0xb4, 0x70, 
	                        0x64, 0xb3, 0x2d, 0x33},
						  { 0x82, 0xac, 0xb3, 0x8c,
						    0x5d, 0x7a, 0x3c, 0x78,
							0xd9, 0x8f, 0x15, 0x3c,
							0xa3, 0xf9, 0xde, 0x7c} };
	uint8_t iv_1[] = {0x21, 0x43, 0x65, 0x87};
	uint8_t iv_2[] = {0x9c, 0x53, 0x2f, 0x8a, 
	                  0xc3, 0xea, 0x4b, 0x2e, 
	                  0xa0, 0xf5, 0x96, 0x40, 
	                  0x30, 0x83, 0x77, 0xcc};
	uint8_t data[16];
	uint8_t i;
	mickey128_ctx_t ctx;
	cli_putstr_P(PSTR("\r\n-=Reference test=-\r\n"));
	
	cli_putstr_P(PSTR("Test 1:\r\n key:    "));
	cli_hexdump(key[0], 16);
	cli_putstr_P(PSTR("\r\n iv:     "));
	cli_hexdump(iv_1, 4);
	mickey128_init(&(key[0][0]), 128, iv_1, 4*8, &ctx);
	for(i=0; i<16; ++i){
		data[i] = mickey128_getbyte(&ctx);
	}
	cli_putstr_P(PSTR("\r\n stream: "));
	cli_hexdump(data, 16);
	
	cli_putstr_P(PSTR("\r\n\r\nTest 2:\r\n key:    "));
	cli_hexdump(key[1], 16);
	cli_putstr_P(PSTR("\r\n iv:     "));
	cli_hexdump(iv_2, 16);
	mickey128_init(key[1], 128, iv_2, 16*8, &ctx);
	for(i=0; i<16; ++i){
		data[i] = mickey128_getbyte(&ctx);
	}
	cli_putstr_P(PSTR("\r\n stream: "));
	cli_hexdump(data, 16);
	
	cli_putstr_P(PSTR("\r\n\r\nTest 3:\r\n key:    "));
	cli_hexdump(key[2], 16);
	cli_putstr_P(PSTR("\r\n iv:     "));
	mickey128_init(key[2], 128, NULL, 0, &ctx);
	for(i=0; i<16; ++i){
		data[i] = mickey128_getbyte(&ctx);
	}
	cli_putstr_P(PSTR("\r\n stream: "));
	cli_hexdump(data, 16);

	cli_putstr_P(PSTR("\r\n\r\nIteration test:\r\n key:    "));
	cli_hexdump(key[3], 16);
	mickey128_init(key[3], 128, NULL, 0, &ctx);
	uint16_t j;
	for(j=0; j<1000; ++j){
		for(i=0; i<16; ++i)
			key[0][i] = mickey128_getbyte(&ctx);
		for(i=0; i<4; ++i)
			iv_1[i] = mickey128_getbyte(&ctx);
		mickey128_init(key[0], 128, iv_1, 32, &ctx);
	}
		for(i=0; i<16; ++i){
		data[i] = mickey128_getbyte(&ctx);
	}
	cli_putstr_P(PSTR("\r\n stream: "));
	cli_hexdump(data, 16);
	cli_putstr_P(PSTR("\r\n"));

}



/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_mickey128},
	{ test_str,        NULL, testrun_ref_mickey128},
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
