/* main-dsa-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)

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
 * DSA test-suit
 *
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "noekeon.h"
#include "noekeon_prng.h"
#include "bigint.h"
#include "bigint_io.h"
#include "dsa.h"
#include "dsa_key_blob.h"

#include "cli.h"
#include "performance_test.h"
#include "hfal_sha1.h"
#include "base64_enc.h"
#include "base64_dec.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "DSA";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

dsa_ctx_t dsa_ctx;

void load_fix_dsa(void){
	load_dsa_key_blob(&dsa_ctx);
}

void dsa_print_item(bigint_t* a, PGM_P pstr){
	uint8_t *p;
	cli_putstr_P(PSTR("\r\n"));
	cli_putstr_P(pstr);
	cli_putstr_P(PSTR(": "));
	uint16_t i;
	p = a->wordv + a->length_B -1;
	for(i=0; i<a->length_B-1; ++i){
		if(i%16==0){
			cli_putstr_P(PSTR("\r\n    "));
		}
		cli_hexdump(p, 1);
		cli_putc(':');
		--p;
	}
	if(i%16==0){
		cli_putstr_P(PSTR("\r\n    "));
	}
	cli_hexdump(p, 1);
}

void dsa_print_signature_b64(dsa_signature_t* s){
	uint16_t size_r, size_s, size_o, i,j;
	size_r = s->r.length_B +2;
	size_s = s->s.length_B +2;
	size_o = size_r + size_s +2;
	uint8_t bin_b[size_o];
	bin_b[0] = 0x30;
	bin_b[1] = size_o -2;
	bin_b[2] = 0x02;
	bin_b[3] = size_r-2;
	j=4;
	for(i=s->r.length_B; i>0;  --i){
		bin_b[j++] = s->r.wordv[i-1];
	}
	bin_b[j++] = 0x02;
	bin_b[j++] = size_s -2;
	for(i=s->s.length_B; i>0;  --i){
		bin_b[j++] = s->s.wordv[i-1];
	}
	char b64_b[size_o*4/3+5];
	base64enc(b64_b, bin_b, size_o);
	cli_putstr(b64_b);
}

void dsa_print_ctx(dsa_ctx_t* ctx){
	dsa_print_item(&(ctx->priv), PSTR("private"));
	dsa_print_item(&(ctx->pub), PSTR("public"));
	dsa_print_item(&(ctx->domain.p), PSTR("P"));
	dsa_print_item(&(ctx->domain.q), PSTR("Q"));
	dsa_print_item(&(ctx->domain.g), PSTR("G"));
}

void dsa_print_signature(const dsa_signature_t* sig){
	cli_putstr_P(PSTR("\r\nDSA-Signature:\r\n r:"));
	bigint_print_hex(&(sig->r));
	cli_putstr_P(PSTR("\r\n s:"));
	bigint_print_hex(&(sig->s));
}

void quick_test(void){
	dsa_signature_t dsa_sig;
	uint8_t i, t=0, message[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
	load_fix_dsa();
	uint8_t dsa_sig_s_b[dsa_ctx.domain.q.length_B],
	        dsa_sig_r_b[dsa_ctx.domain.q.length_B];
	dsa_print_ctx(&dsa_ctx);
	dsa_sig.r.wordv = dsa_sig_r_b;
	dsa_sig.s.wordv = dsa_sig_s_b;
	cli_putstr_P(PSTR("\r\n\r\n=== DSA QUICK TEST ==="));
	for(i=0; i<8; ++i){
		cli_putstr_P(PSTR("\r\n"));
		cli_putc('1'+i);
		cli_putstr_P(PSTR(": message: "));
		if (i){
			cli_hexdump(message, i);
		}else{
			cli_putstr_P(PSTR("<empty>"));
		}
		cli_putstr_P(PSTR("\r\n computing signature ... "));
		dsa_sign_message(&dsa_sig, message, i*8, &sha1_desc, &dsa_ctx, random8);
		dsa_print_signature(&dsa_sig);
		cli_putstr_P(PSTR("\r\n base64:\r\n--- SIGNATURE ---\r\n "));
		dsa_print_signature_b64(&dsa_sig);
		cli_putstr_P(PSTR("\r\n verifying signature ... "));
		t = dsa_verify_message(&dsa_sig, message, i*8, &sha1_desc, &dsa_ctx);
		cli_putstr_P(PSTR("\r\n verification: "));
		if(t==DSA_SIGNATURE_OK){
			cli_putstr_P(PSTR("[PASS]"));
		}else{
			cli_putstr_P(PSTR("[FAIL]"));
		}
	}
}

void reset_prng(void){
	uint8_t buf[16];
	memset(buf, 0, 16);
	random_seed(buf);
	cli_putstr_P(PSTR("\r\nPRNG reset"));
}

void testrun_performance_bigint(void){

}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char echo_test_str[]        PROGMEM = "echo-test";
const char reset_prng_str[]       PROGMEM = "reset-prng";
const char quick_test_str[]       PROGMEM = "quick-test";
const char performance_str[]      PROGMEM = "performance";
const char echo_str[]             PROGMEM = "echo";

const const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ reset_prng_str,       NULL, reset_prng                    },
	{ quick_test_str,       NULL, quick_test                    },
	{ performance_str,      NULL, testrun_performance_bigint    },
	{ echo_str,         (void*)1, (void_fpt)echo_ctrl           },
	{ NULL,                 NULL, NULL                          }
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
