/* main-bigint-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008, 2009, 2010  Daniel Otte (daniel.otte@rub.de)

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
 * bigint test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "noekeon.h"
#include "noekeon_prng.h"
#include "bigint.h"
#include "bigint_io.h"

#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "BigInt";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void test_echo_bigint(void){
	bigint_t a;
	cli_putstr_P(PSTR("\r\necho test\r\n"));
	for(;;){
		cli_putstr_P(PSTR("\r\nenter hex number:"));
		if(bigint_read_hex_echo(&a)){
			cli_putstr_P(PSTR("\r\n end echo test"));
			return;
		}
		cli_putstr_P(PSTR("\r\necho: "));
		bigint_print_hex(&a);
		cli_putstr_P(PSTR("\r\n"));
		free(a.wordv);
	}
}

void test_add_bigint(void){
	bigint_t a, b, c;
	cli_putstr_P(PSTR("\r\nadd test\r\n"));
	for(;;){
		cli_putstr_P(PSTR("\r\nenter a:"));
		if(bigint_read_hex_echo(&a)){
			cli_putstr_P(PSTR("\r\n end add test"));
			return;
		}
		cli_putstr_P(PSTR("\r\nenter b:"));
		if(bigint_read_hex_echo(&b)){
			free(a.wordv);
			cli_putstr_P(PSTR("\r\n end add test"));
			return;
		}
		cli_putstr_P(PSTR("\r\n "));
		bigint_print_hex(&a);
		cli_putstr_P(PSTR(" + "));
		bigint_print_hex(&b);
		cli_putstr_P(PSTR(" = "));
		uint8_t *c_b;
		c_b = malloc(((a.length_B>b.length_B)?a.length_B:b.length_B)+2);
		if(c_b==NULL){
			cli_putstr_P(PSTR("\n\rERROR: Out of memory!"));
			free(a.wordv);
			free(b.wordv);
			continue;
		}
		c.wordv = c_b;
		bigint_add_s(&c, &a, &b);
		bigint_print_hex(&c);
		cli_putstr_P(PSTR("\r\n"));
		free(a.wordv);
		free(b.wordv);
		free(c_b);
	}
}

void test_add_scale_bigint(void){
	bigint_t a, b, c;
	uint16_t scale;
	cli_putstr_P(PSTR("\r\nadd-scale test\r\n"));
	for(;;){
		cli_putstr_P(PSTR("\r\nenter a:"));
		if(bigint_read_hex_echo(&a)){
			cli_putstr_P(PSTR("\r\n end add-scale test"));
			return;
		}
		cli_putstr_P(PSTR("\r\nenter b:"));
		if(bigint_read_hex_echo(&b)){
			cli_putstr_P(PSTR("\r\n end add-scale test"));
			return;
		}
		cli_putstr_P(PSTR("\r\nenter scale:"));
		{
			char str[8];
			cli_getsn_cecho(str, 7);
			scale = atoi(str);
		}
	/*
		if(bigint_read_hex_echo(&scale)){
			free(scale.wordv);
			cli_putstr_P(PSTR("\r\n end add test"));
			return;
		}
	*/
		uint8_t *c_b;
		c_b = malloc(((a.length_B>(b.length_B+scale))?a.length_B:(b.length_B+scale))+2);
		if(c_b==NULL){
			cli_putstr_P(PSTR("\n\rERROR: Out of memory!"));
			free(a.wordv);
			free(b.wordv);
			continue;
		}
		c.wordv = c_b;
		bigint_copy(&c, &a);
		bigint_add_scale_u(&c, &b, scale);
		cli_putstr_P(PSTR("\r\n "));
		bigint_print_hex(&a);
		cli_putstr_P(PSTR(" + "));
		bigint_print_hex(&b);
		cli_putstr_P(PSTR("<<8*"));
		cli_hexdump_rev(&scale, 2);
		cli_putstr_P(PSTR(" = "));
		bigint_print_hex(&c);
		cli_putstr_P(PSTR("\r\n"));
		free(a.wordv);
		free(b.wordv);
		free(c_b);
	}
}

void test_mul_bigint(void){
	bigint_t a, b, c;
	cli_putstr_P(PSTR("\r\nmul test\r\n"));
	for(;;){
		cli_putstr_P(PSTR("\r\nenter a:"));
		if(bigint_read_hex_echo(&a)){
			cli_putstr_P(PSTR("\r\n end mul test"));
			return;
		}
		cli_putstr_P(PSTR("\r\nenter b:"));
		if(bigint_read_hex_echo(&b)){
			free(a.wordv);
			cli_putstr_P(PSTR("\r\n end mul test"));
			return;
		}
		cli_putstr_P(PSTR("\r\n "));
		bigint_print_hex(&a);
		cli_putstr_P(PSTR(" * "));
		bigint_print_hex(&b);
		cli_putstr_P(PSTR(" = "));
		uint8_t *c_b;
		c_b = malloc((((a.length_B>b.length_B)?a.length_B:b.length_B)+1)*2);
		if(c_b==NULL){
			cli_putstr_P(PSTR("\n\rERROR: Out of memory!"));
			free(a.wordv);
			free(b.wordv);
			continue;
		}
		c.wordv = c_b;
		bigint_mul_s(&c, &a, &b);
		bigint_print_hex(&c);
		cli_putstr_P(PSTR("\r\n"));
		free(a.wordv);
		free(b.wordv);
		free(c_b);
	}
}

void test_square_bigint(void){
	bigint_t a, c;
	cli_putstr_P(PSTR("\r\nsquare test\r\n"));
	for(;;){
		cli_putstr_P(PSTR("\r\nenter a:"));
		if(bigint_read_hex_echo(&a)){
			cli_putstr_P(PSTR("\r\n end square test"));
			return;
		}
		cli_putstr_P(PSTR("\r\n "));
		bigint_print_hex(&a);
		cli_putstr_P(PSTR("**2 = "));
		uint8_t *c_b;
		c_b = malloc(a.length_B*2);
		if(c_b==NULL){
			cli_putstr_P(PSTR("\n\rERROR: Out of memory!"));
			free(a.wordv);
			continue;
		}
		c.wordv = c_b;
		bigint_square(&c, &a);
		bigint_print_hex(&c);
		cli_putstr_P(PSTR("\r\n"));
		free(a.wordv);
		free(c_b);
	}
}

void test_reduce_bigint(void){
	bigint_t a, b;
	cli_putstr_P(PSTR("\r\nreduce test\r\n"));
	for(;;){
		cli_putstr_P(PSTR("\r\nenter a:"));
		if(bigint_read_hex_echo(&a)){
			cli_putstr_P(PSTR("\r\n end reduce test"));
			return;
		}
		cli_putstr_P(PSTR("\r\nenter b:"));
		if(bigint_read_hex_echo(&b)){
			free(a.wordv);
			cli_putstr_P(PSTR("\r\n end reduce test"));
			return;
		}
		cli_putstr_P(PSTR("\r\n "));
		bigint_print_hex(&a);
		cli_putstr_P(PSTR(" % "));
		bigint_print_hex(&b);
		cli_putstr_P(PSTR(" = "));
		bigint_reduce(&a, &b);
		bigint_print_hex(&a);
		cli_putstr_P(PSTR("\r\n"));
		free(a.wordv);
		free(b.wordv);
	}
}
/* d = a**b % c */
void test_expmod_bigint(void){
	bigint_t a, b, c, d;
	uint8_t *d_b;
	cli_putstr_P(PSTR("\r\nreduce test\r\n"));
	for(;;){
		cli_putstr_P(PSTR("\r\nenter a:"));
		if(bigint_read_hex_echo(&a)){
			cli_putstr_P(PSTR("\r\n end expmod test"));
			return;
		}
		cli_putstr_P(PSTR("\r\nenter b:"));
		if(bigint_read_hex_echo(&b)){
			free(a.wordv);
			cli_putstr_P(PSTR("\r\n end expmod test"));
			return;
		}
		cli_putstr_P(PSTR("\r\nenter c:"));
		if(bigint_read_hex_echo(&c)){
			free(a.wordv);
			free(b.wordv);
			cli_putstr_P(PSTR("\r\n end expmod test"));
			return;
		}
		d_b = malloc(c.length_B);
		if(d_b==NULL){
			cli_putstr_P(PSTR("\n\rERROR: Out of memory!"));
			free(a.wordv);
			free(b.wordv);
			free(c.wordv);
			continue;
		}
		d.wordv = d_b;
		cli_putstr_P(PSTR("\r\n "));
		bigint_print_hex(&a);
		cli_putstr_P(PSTR("**"));
		bigint_print_hex(&b);
		cli_putstr_P(PSTR(" % "));
		bigint_print_hex(&c);
		cli_putstr_P(PSTR(" = "));
		bigint_expmod_u(&d, &a, &b, &c);
		bigint_print_hex(&d);
		cli_putstr_P(PSTR("\r\n"));
		free(a.wordv);
		free(b.wordv);
		free(c.wordv);
		free(d.wordv);

	}
}

void test_gcdext_bigint(void){
	bigint_t a, b, c, d, e;
	cli_putstr_P(PSTR("\r\ngcdext test\r\n"));
	for(;;){
		cli_putstr_P(PSTR("\r\nenter a:"));
		if(bigint_read_hex_echo(&a)){
			cli_putstr_P(PSTR("\r\n end gcdext test"));
			return;
		}
		cli_putstr_P(PSTR("\r\nenter b:"));
		if(bigint_read_hex_echo(&b)){
			free(a.wordv);
			cli_putstr_P(PSTR("\r\n end gcdext test"));
			return;
		}
		c.wordv = malloc((a.length_B<b.length_B)?a.length_B:b.length_B);
		d.wordv = malloc(1+(a.length_B>b.length_B)?a.length_B:b.length_B);
		e.wordv = malloc(1+(a.length_B>b.length_B)?a.length_B:b.length_B);

		cli_putstr_P(PSTR("\r\n gcdext( "));
		bigint_print_hex(&a);
		cli_putstr_P(PSTR(", "));
		bigint_print_hex(&b);
		cli_putstr_P(PSTR(") => "));
		bigint_gcdext(&c, &d, &e, &a, &b);
		cli_putstr_P(PSTR("a = "));
		bigint_print_hex(&d);
		cli_putstr_P(PSTR("; b = "));
		bigint_print_hex(&e);
		cli_putstr_P(PSTR("; gcd = "));
		bigint_print_hex(&c);

		cli_putstr_P(PSTR("\r\n"));
		free(a.wordv);
		free(b.wordv);
		free(c.wordv);
		free(d.wordv);
		free(e.wordv);
	}
}

void test_simple(void){
	bigint_t a, b, c;
	uint8_t a_b[1], b_b[1], c_b[2];
	a.wordv=a_b;
	b.wordv=b_b;
	c.wordv=c_b;
	a.length_B = 1;
	b.length_B = 1;
	a_b[0] = 1;
	b_b[0] = 2;
	bigint_add_u(&c, &a, &b);
	cli_putstr_P(PSTR("\r\n 1+2="));
	bigint_print_hex(&c);
}
/*
void test_mul_simple(void){
	bigint_t a, b, c;
	uint8_t a_b[5] = {0x79, 0x36, 0x9e, 0x72, 0xec};
	uint8_t b_b[5] = {0x4a, 0x47, 0x0d, 0xec, 0xfd};
	uint8_t c_b[12];
	a.wordv=a_b;
	b.wordv=b_b;
	c.wordv=c_b;
	a.length_B = 5;
	b.length_B = 5;
	bigint_adjust(&a);
	bigint_adjust(&b);
	bigint_mul_s(&c, &a, &b);
	cli_putstr_P(PSTR("\r\n test: "));
	bigint_print_hex(&c);
}
*/

// -3d1d 6db7 8251 f371 * -7a18 3791 d18b b7c5 = 1d25ce4fdf93390f8d6c709f4d711cf5
// -20538248dece6d29068d * 400b1411b874f81394c6 = -81646b193d95136a6fedb73cee6d30c39fb950e
// -BC8B 7D53 4921 853D * 0DDA 6044 00CE DDE6   =  -a33eb0c5847db8837589c22db395dce
void test_mul_simple(void){
	bigint_t a, b, c;

//	uint8_t a_b[10] = {0x8d, 0x06, 0x29, 0x6d, 0xce, 0xde, 0x48, 0x82, 0x53, 0x20};
//	uint8_t b_b[10] = {0xc6, 0x94, 0x13, 0xf8, 0x74, 0xb8, 0x11, 0x14, 0x0b, 0x40};
	uint8_t a_b[8] = {0x3d, 0x85, 0x21, 0x49, 0x53, 0x7d, 0x8b, 0xbc};
	uint8_t b_b[8] = {0xe6, 0xdd, 0xce, 0x00, 0x44, 0x60, 0xda, 0x0d};

	uint8_t c_b[16];
	a.wordv=a_b;
	b.wordv=b_b;
	c.wordv=c_b;
	a.length_B = 8;
	b.length_B = 8;
	a.info=0x80;
	bigint_adjust(&a);
	bigint_adjust(&b);
	bigint_mul_s(&c, &a, &b);
	cli_putstr_P(PSTR("\r\n test: "));
	bigint_print_hex(&a);
	cli_putstr_P(PSTR(" * "));
	bigint_print_hex(&b);
	cli_putstr_P(PSTR(" = "));
	bigint_print_hex(&c);
}

// f4 b86a 2220 0774 437d 70e6 **2 = e9f00f29ca1c876a7a682bd1e04f6925caffd6660ea4
/*
const uint8_t square_test_data[] PROGMEM = {
	0xA0, 0x3C, 0x23, 0x9F, 0x7A, 0xFC, 0x60, 0xEB, 0x96, 0xC2, 0xA8, 0xAC, 0xC3, 0xC9, 0x9E, 0xEC,
	0x4A, 0xF0, 0x1C, 0xB2, 0x36, 0x68, 0xD6, 0x4D, 0x3E, 0x4F, 0x8E, 0x55, 0xEA, 0x52, 0x46, 0x68,
	0x6E, 0x18, 0x88, 0x37, 0x03, 0x70, 0xBD, 0x01, 0x60, 0xE2, 0xD6, 0x12, 0xA0, 0x0E, 0xD2, 0x72,
	0x0D, 0x9D, 0x9F, 0x03, 0xC5, 0x81, 0xCA, 0x6E, 0x88, 0x1E, 0xF5, 0xD8, 0x14, 0x15, 0x30, 0xEB,
	0x28, 0x7C, 0x80, 0x07, 0x34, 0x05, 0x5D, 0xAA, 0xDC, 0xA8, 0xAA, 0x88, 0xC5, 0xE5, 0xC9, 0xFE,
	0x9C, 0xA1, 0xCE, 0xC2, 0x09, 0x0D, 0xC4, 0xC8, 0xD3, 0xE7, 0x3A, 0xF3, 0xEF, 0xDF, 0xAE, 0x07,
	0xEC, 0xC7, 0x83, 0x50, 0x9F, 0x6D, 0xB9, 0x28, 0x77, 0xC0, 0xFE, 0x69, 0xB2, 0x2E, 0x55, 0x90,
	0x50, 0xED, 0xE0, 0xA1, 0x4D, 0x3D, 0x38, 0xC9, 0x0E, 0xCD, 0x04, 0x3B, 0x64, 0x3F, 0x56, 0xC5,
	0xC3, 0x9E, 0x89, 0x81, 0x44, 0x60, 0xBA, 0x8E, 0x88, 0xA4, 0xA3, 0x42, 0x7B, 0x06, 0x93, 0x1C,
	0x6B, 0x04, 0x29, 0xF9, 0xDD, 0xFF, 0xB0, 0x48, 0x2F, 0x6D, 0xD1, 0x0F, 0x7D, 0xA6, 0x26, 0xD8,
	0xEF, 0x5E, 0x04, 0x18, 0xD1, 0x61, 0x46, 0x37, 0x87, 0xE2, 0x97, 0xDF, 0x10, 0xB4, 0x9A, 0x39,
	0xB1, 0xD0, 0xCA, 0x91, 0x48, 0x1E, 0x5D, 0xA1, 0x38, 0x89, 0x02, 0xC1, 0x49, 0x86, 0xB7, 0xAE,
	0x69, 0x20, 0xFA, 0x0E, 0x39, 0xDA, 0xA5, 0xEF, 0x7F, 0xB2, 0x81, 0xB8, 0xC0, 0x3A, 0xF8, 0xDB,
	0xBC, 0x45, 0xF6, 0xDA, 0xCD, 0xBE, 0x27, 0xBE, 0xF6, 0x20, 0x79, 0xF3, 0xC3, 0xC8, 0xFF, 0x85,
	0x43, 0x9F, 0xB1, 0x9B, 0x72, 0x88, 0xDD, 0xA4, 0x0D, 0xFC, 0xC6, 0xB5, 0x74, 0x67, 0x29, 0xF5
};
*/

void test_square_simple(void){
	bigint_t a, c;

	uint8_t a_b[11] = {0xe6, 0x70, 0x7d, 0x43, 0x74, 0x07, 0x20, 0x22, 0x6a, 0xb8, 0xf4};
	uint8_t c_b[22];
	a.wordv=a_b;
	c.wordv=c_b;
	a.length_B = 11;
	a.info=0x00;
	bigint_adjust(&a);
	bigint_square(&c, &a);
	cli_putstr_P(PSTR("\r\n test: "));
	bigint_print_hex(&a);
	cli_putstr_P(PSTR("**2 = "));
	bigint_print_hex(&c);
}

// [fail (c)]:  A862 % 2752 = 0D1A ; should a862 % 2752 = b1a
void test_reduce_simple(void){
	bigint_t a, b, c;

	uint8_t a_b[2] = {0x62, 0xA8};
	uint8_t b_b[2] = {0x52, 0x27};
	uint8_t c_b[2];
	a.wordv=a_b;
	a.length_B = 2;
	a.info=0x00;
	bigint_adjust(&a);
	b.wordv=b_b;
	b.length_B = 2;
	b.info=0x00;
	bigint_adjust(&b);
	c.wordv = c_b;
	bigint_copy(&c, &a);
	bigint_reduce(&c, &b);
	cli_putstr_P(PSTR("\r\n test: "));
	bigint_print_hex(&a);
	cli_putstr_P(PSTR(" % "));
	bigint_print_hex(&b);
	cli_putstr_P(PSTR(" = "));
	bigint_print_hex(&c);
}

/*  gcdext( B5DDAD, 6CBBC2) */
/*  gcdext( CD319349, 9EFD76CC) */
/*  gcdext( 1609000771, 6FAC577D72) */
/*  */
void test_gcdext_simple(void){
	bigint_t a, b, c, d, e;

	uint8_t a_b[5] = {0x71, 0x07, 0x00, 0x09, 0x16};
	uint8_t b_b[5] = {0x72, 0x7D, 0x57, 0xAC, 0X6F};
	uint8_t c_b[6], d_b[6], e_b[6];
	a.wordv=a_b;
	a.length_B = 5;
	a.info=0x00;
	bigint_adjust(&a);
	b.wordv=b_b;
	b.length_B = 5;
	b.info=0x00;
	bigint_adjust(&b);
	c.wordv = c_b;
	d.wordv = d_b;
	e.wordv = e_b;
	bigint_gcdext(&c, &d, &e, &a, &b);
	cli_putstr_P(PSTR("\r\n test: gcd( "));
	bigint_print_hex(&a);
	cli_putstr_P(PSTR(", "));
	bigint_print_hex(&b);
	cli_putstr_P(PSTR(") => a =  "));
	bigint_print_hex(&d);
	cli_putstr_P(PSTR("; b =  "));
	bigint_print_hex(&e);
	cli_putstr_P(PSTR("; gcd =  "));
	bigint_print_hex(&c);
}

void testrun_performance_bigint(void){

}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char echo_test_str[]        PROGMEM = "echo-test";
const char add_test_str[]         PROGMEM = "add-test";
const char add_scale_test_str[]   PROGMEM = "add-scale-test";
const char mul_test_str[]         PROGMEM = "mul-test";
const char square_test_str[]      PROGMEM = "square-test";
const char reduce_test_str[]      PROGMEM = "reduce-test";
const char expmod_test_str[]      PROGMEM = "expmod-test";
const char gcdext_test_str[]      PROGMEM = "gcdext-test";
const char quick_test_str[]       PROGMEM = "quick-test";
const char performance_str[]      PROGMEM = "performance";
const char echo_str[]             PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ add_test_str,         NULL, test_add_bigint               },
	{ add_scale_test_str,   NULL, test_add_scale_bigint         },
	{ mul_test_str,         NULL, test_mul_bigint               },
	{ square_test_str,      NULL, test_square_bigint            },
	{ reduce_test_str,      NULL, test_reduce_bigint            },
	{ expmod_test_str,      NULL, test_expmod_bigint            },
	{ gcdext_test_str,      NULL, test_gcdext_bigint            },
	{ quick_test_str,       NULL, test_gcdext_simple            },
	{ echo_test_str,        NULL, test_echo_bigint              },
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
