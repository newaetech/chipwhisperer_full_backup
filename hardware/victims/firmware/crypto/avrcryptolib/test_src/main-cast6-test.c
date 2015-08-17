/*
 * rc6 test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "cast6.h"
#include "cli.h"
#include "performance_test.h"
#include "bcal-performance.h"
#include "bcal-nessie.h"
#include "bcal_cast6.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

char* algo_name = "CAST-256";

const bcdesc_t* const algolist[] PROGMEM = {
	(bcdesc_t*)&cast6_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/


void testrun_nessie_cast6(void){
	bcal_nessie_multiple(algolist);
}

void testrun_rfc_cast6(void){
	cli_putstr_P(PSTR("\r\n testvalues from rfc-2612\r\n"));
	uint8_t key[32], data[16];
	cast6_ctx_t ctx;
	memcpy_P(key, PSTR("\x23\x42\xbb\x9e\xfa\x38\x54\x2c"
	                   "\x0a\xf7\x56\x47\xf2\x9f\x61\x5d"), 16);
	memset(data, 0, 16);
	
	cli_putstr_P(PSTR("\r\n key: "));
	cli_hexdump(key, 16);
	cli_putstr_P(PSTR("\r\n PT:  "));
	cli_hexdump(data, 16);
	cast6_init(key, 128, &ctx);
	cast6_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\n CT:  "));
	cli_hexdump(data, 16);
	cast6_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\n PT:  "));
	cli_hexdump(data, 16);
	
	cli_putstr_P(PSTR("\r\n\r\n"));

	memcpy_P(key, PSTR("\x23\x42\xbb\x9e\xfa\x38\x54\x2c"
	                   "\xbe\xd0\xac\x83\x94\x0a\xc2\x98"
				       "\xba\xc7\x7a\x77\x17\x94\x28\x63"), 24);
	
	cli_putstr_P(PSTR("\r\n key: "));
	cli_hexdump(key, 24);
	cli_putstr_P(PSTR("\r\n PT:  "));
	cli_hexdump(data, 16);
	cast6_init(key, 192, &ctx);
	cast6_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\n CT:  "));
	cli_hexdump(data, 16);
	cast6_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\n PT:  "));
	cli_hexdump(data, 16);
	
	cli_putstr_P(PSTR("\r\n\r\n"));
	
	memcpy_P(key, PSTR("\x23\x42\xbb\x9e\xfa\x38\x54\x2c"
	                   "\xbe\xd0\xac\x83\x94\x0a\xc2\x98"
				       "\x8d\x7c\x47\xce\x26\x49\x08\x46"
				       "\x1c\xc1\xb5\x13\x7a\xe6\xb6\x04"), 32);
	cli_putstr_P(PSTR("\r\n key: "));
	cli_hexdump(key, 32);
	cli_putstr_P(PSTR("\r\n PT:  "));
	cli_hexdump(data, 16);
	cast6_init(key, 256, &ctx);
	cast6_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\n CT:  "));
	cli_hexdump(data, 16);
	cast6_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\n PT:  "));
	cli_hexdump(data, 16);
	
	cli_putstr_P(PSTR("\r\n\r\n"));
}

void testrun_performance_cast6(void){
	bcal_performance_multiple(algolist);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_cast6 },
	{ test_str,        NULL, testrun_rfc_cast6},
	{ performance_str, NULL, testrun_performance_cast6},
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
