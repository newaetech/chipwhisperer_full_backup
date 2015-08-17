/* cmacvs.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006 2007 2008 2009  Daniel Otte (daniel.otte@rub.de)

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
/**
 * \file	cmacvs.c
 * \author  Daniel Otte
 * \date    2010-02-02
 * \license	GPLv3 or later
 *
 */

#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "blockcipher_descriptor.h"
#include "bcal-basic.h"
#include "bcal-cmac.h"
#include "cmacvs.h"
#include "string-extras.h"
#include "cli.h"


#ifdef DEBUG
#  undef DEBUG
#endif

#define DEBUG 0

#if DEBUG
#  include "config.h"
#  include <util/delay.h>
#endif

bcdesc_t*  cmacvs_algo=NULL;
bcdesc_t** cmacvs_algolist=NULL;

void cmacvs_listalgos(void){
	char option = 'a';

	bcdesc_t* t;
	uint8_t i=0;
	cli_putstr_P(PSTR("\r\nthe following algorithms are available:\r\n"));
	while(option<='z' && (t=(bcdesc_t*)pgm_read_word(&(cmacvs_algolist[i])))){
		cli_putc('\t');
		cli_putc((t==cmacvs_algo)?'*':' ');
		cli_putc(option++);
		cli_putstr_P(PSTR(":\t"));
		cli_putstr_P((void*)(pgm_read_word(&(t->name))));
		cli_putstr_P(PSTR("\r\n"));
		i++;
	}
}

void cmacvs_setalgo(char* param){
	param = strstrip(param);
	if(param[1]=='\0'){ /* single letter specified */
		uint8_t i,option = param[0]-'a';

		if(!cmacvs_algolist){
			cli_putstr_P(PSTR("\r\nERROR: cmacvs_algolist not set!"));
			return;
		}
		for(i=0; i<=option; ++i){
			if((void*)pgm_read_word(&(cmacvs_algolist[i]))==NULL){
				cli_putstr_P(PSTR("\r\nERROR: invalid selection!"));
				return;
			}
		}
		cmacvs_algo=(bcdesc_t*)pgm_read_word(&(cmacvs_algolist[option]));
	} else { /* name specifyed */
		bcdesc_t* t=NULL;
		uint8_t i=0;
		while((t=(bcdesc_t*)pgm_read_word(&(cmacvs_algolist[i]))) &&
		       strcasecmp_P(param, (void*)pgm_read_word(&(t->name))))
			++i;
		if(t){
			cmacvs_algo=t;
		}else{
			cli_putstr_P(PSTR("\r\nERROR: could not find \""));
			cli_putstr(param);
			cli_putstr_P(PSTR("\"!"));
		}
	}
}

typedef struct {
	uint16_t buffer_idx;
	uint16_t buffersize_B;
	uint32_t blocks;
	bcal_cmac_ctx_t ctx;
	uint8_t* buffer;
	uint8_t  in_byte;
} cmacvs_ctx_t;

static cmacvs_ctx_t cmacvs_ctx;

uint8_t buffer_add(char c){
	uint8_t v,t;
	if(cmacvs_ctx.buffer_idx==cmacvs_ctx.buffersize_B){
		bcal_cmac_nextBlock(&(cmacvs_ctx.ctx), cmacvs_ctx.buffer);
		++cmacvs_ctx.blocks;
		cmacvs_ctx.buffer_idx=0;
		cmacvs_ctx.in_byte=0;
		cli_putc('.');
		memset(cmacvs_ctx.buffer, 0, cmacvs_ctx.buffersize_B);
	}
	if(c>='0' && c<='9'){
		v=c-'0';
	}else{
		c &= (uint8_t)~('a' ^ 'A');
		if(c>='A' && c<='F'){
			v=c-'A'+10;
		}else{
			return 1;
		}
	}
	t=cmacvs_ctx.buffer[cmacvs_ctx.buffer_idx];
	if(cmacvs_ctx.in_byte){
		t |= v;
		cmacvs_ctx.buffer[cmacvs_ctx.buffer_idx]=t;
		cmacvs_ctx.buffer_idx++;
		cmacvs_ctx.in_byte = 0;
	}else{
		t |= v<<4;
		cmacvs_ctx.buffer[cmacvs_ctx.buffer_idx]=t;
		cmacvs_ctx.in_byte = 1;
	}
	return 0;
}

int32_t getValue_P(PGM_P key){
	uint32_t val=0;
	char instr[21];
	char* str2;
	for(;;){
		memset(instr, 0, 21);
		cli_getsn_cecho(instr, 20);
		str2 = strstrip(instr);
		if(!strncasecmp_P(str2, key, strlen_P(key))){
			while(*str2 && *str2!='=')
				str2++;
			if(*str2=='='){
				do{
					str2++;
				}while(*str2 && !isdigit(*str2));
				val=(uint32_t)strtoul(str2, NULL, 10);
				return val;
			}
		} else {
			if(!strncasecmp_P(str2, PSTR("EXIT"), 4)){
				cli_putstr_P(PSTR("\r\n got exit ..."));
				return -1;
			}
		}
	}
	return -2;
}

uint8_t getKey(void* key_buffer, uint8_t klen_B){
	char c;
	uint8_t v,i=0;
	memset(key_buffer, 0x00, klen_B);
	do{
		c = cli_getc_cecho();
	}while((c|('a'^'A'))!='k');
	do{
		c = cli_getc_cecho();
	}while((c|('a'^'A'))!='e');
	do{
		c = cli_getc_cecho();
	}while((c|('a'^'A'))!='y');
	do{
		c = cli_getc_cecho();
	}while(c!='=');
	klen_B *= 2;
	while(klen_B){
		v = 0x10;
		c = cli_getc_cecho();
		if(c>='0' && c<='9'){
			v = c-'0';
		}else{
			c |= 'A'^'a';
			if(c>='a' && c<='f'){
				v= c-'a'+10;
			}
		}
		if(v<0x10){
			if((i&1)==0){
				v<<=4;
			}
			((uint8_t*)key_buffer)[i/2] |= v;
			++i;
			--klen_B;
		}
	}
	return 0;
}

uint8_t getMac(void* mac_buffer, uint8_t mlen_B){
	char c;
	uint8_t v,i=0;
	memset(mac_buffer, 0x00, mlen_B);
	do{
		c = cli_getc_cecho();
	}while((c|('a'^'A'))!='m');
	do{
		c = cli_getc_cecho();
	}while((c|('a'^'A'))!='a');
	do{
		c = cli_getc_cecho();
	}while((c|('a'^'A'))!='c');
	do{
		c = cli_getc_cecho();
	}while(c!='=');
	mlen_B *= 2;
	while(mlen_B){
		v = 0x10;
		c = cli_getc_cecho();
		if(c>='0' && c<='9'){
			v = c-'0';
		}else{
			c |= 'A'^'a';
			if(c>='a' && c<='f'){
				v= c-'a'+10;
			}
		}
		if(v<0x10){
			if((i&1)==0){
				v<<=4;
			}
			((uint8_t*)mac_buffer)[i/2] |= v;
			++i;
			--mlen_B;
		}
	}
	return 0;
}

void cmacvs_test1(void){ /* Gen tests */
	int32_t klen, mlen, tlen;
	int32_t expect_input=0;

	if(!cmacvs_algo){
			cli_putstr_P(PSTR("\r\nERROR: select algorithm first!"));
		return;
	}
	char c;
	cmacvs_ctx.buffersize_B=pgm_read_word(&(cmacvs_algo->blocksize_b))/8;
	uint8_t tag[cmacvs_ctx.buffersize_B];
	uint8_t buffer[cmacvs_ctx.buffersize_B+5];
	cmacvs_ctx.buffer = buffer;
	cli_putstr_P(PSTR("\r\nbuffer_size = 0x"));
	cli_hexdump_rev(&(cmacvs_ctx.buffersize_B), 2);
	cli_putstr_P(PSTR(" bytes"));
	for(;;){
		cmacvs_ctx.blocks = 0;
		memset(buffer, 0, cmacvs_ctx.buffersize_B);
		klen = getValue_P(PSTR("Klen"));
		if(klen<0){
			return;
		}
		mlen = getValue_P(PSTR("Mlen"));
		if(mlen<0){
			return;
		}
		tlen = getValue_P(PSTR("Tlen"));
		if(tlen<0){
			return;
		}
		uint8_t key_buffer[klen];
#if DEBUG
		cli_putstr_P(PSTR("\r\nKLen == "));
		cli_hexdump_rev(&klen, 4);
		cli_putstr_P(PSTR("\r\nMLen == "));
		cli_hexdump_rev(&mlen, 4);
		cli_putstr_P(PSTR("\r\nTLen == "));
		cli_hexdump_rev(&tlen, 4);
#endif
		getKey(key_buffer, klen);
		if(mlen==0){
			expect_input=2;
		}else{
			expect_input=mlen*2;
		}
#if DEBUG
		cli_putstr_P(PSTR("\r\nexpected_input == "));
		cli_hexdump_rev(&expect_input, 4);
		if(expect_input==0)
			cli_putstr_P(PSTR("\r\nexpected_input == 0 !!!"));
#endif
		uint8_t ret;
#if DEBUG
		cli_putstr_P(PSTR("\r\n CMAC init"));
		cli_putstr_P(PSTR("\r\n (2) expected_input == "));
		cli_hexdump_rev(&expect_input, 4);
#endif
		ret = bcal_cmac_init(cmacvs_algo, key_buffer, klen*8, &(cmacvs_ctx.ctx));
		if(ret){
			cli_putstr_P(PSTR("\r\n bcal_cmac_init returned with: "));
			cli_hexdump(&ret, 1);
			return;
		}
#if DEBUG
		cli_putstr_P(PSTR("\r\n (3) expected_input == "));
		cli_hexdump_rev(&expect_input, 4);
		cli_putstr_P(PSTR("\r\n"));
#endif
		while((c=cli_getc_cecho())!='M' && c!='m'){
			if(!isspace(c)){
				cli_putstr_P(PSTR("\r\nERROR: wrong input (1) [0x"));
				cli_hexdump(&c, 1);
				cli_putstr_P(PSTR("]!\r\n"));
				bcal_cmac_free(&(cmacvs_ctx.ctx));
				return;
			}
		}
		if((c=cli_getc_cecho())!='s' && c!='S'){
				cli_putstr_P(PSTR("\r\nERROR: wrong input (2)!\r\n"));
				bcal_cmac_free(&(cmacvs_ctx.ctx));
				return;
		}
		if((c=cli_getc_cecho())!='g' && c!='G'){
				cli_putstr_P(PSTR("\r\nERROR: wrong input (3)!\r\n"));
				bcal_cmac_free(&(cmacvs_ctx.ctx));
				return;
		}
		while((c=cli_getc_cecho())!='='){
			if(!isspace(c)){
				cli_putstr_P(PSTR("\r\nERROR: wrong input (4)!\r\n"));
				bcal_cmac_free(&(cmacvs_ctx.ctx));
				return;
			}
		}
#if DEBUG
		cli_putstr_P(PSTR("\r\nparsing started"));
#endif
		cmacvs_ctx.buffer_idx = 0;
		cmacvs_ctx.in_byte    = 0;
		cmacvs_ctx.blocks     = 0;
		while(expect_input>0){
			c=cli_getc_cecho();
#if DEBUG
			cli_putstr_P(PSTR("\r\n\t("));
			cli_hexdump_rev(&expect_input, 4);
			cli_putstr_P(PSTR(") "));
			_delay_ms(500);
#endif
			if(buffer_add(c)==0){
				--expect_input;
			}else{
				if(!isblank((uint16_t)c)){
					cli_putstr_P(PSTR("\r\nERROR: wrong input (5) ("));
					cli_putc(c);
					cli_putstr_P(PSTR(")!\r\n"));
					bcal_cmac_free(&(cmacvs_ctx.ctx));
					return;
				}
			}
		}
#if DEBUG
		cli_putstr_P(PSTR("\r\nBuffer-A:"));
		cli_hexdump_block(buffer, cmacvs_ctx.buffersize_B, 5, 8);

		cli_putstr_P(PSTR("\r\n starting finalisation"));
		cli_putstr_P(PSTR("\r\n\tblocks     == "));
		cli_hexdump_rev(&(cmacvs_ctx.blocks),4);
		cli_putstr_P(PSTR("\r\n\tbuffer_idx == "));
		cli_hexdump_rev(&(cmacvs_ctx.buffer_idx),2);
		cli_putstr_P(PSTR("\r\n\tin_byte    == "));
		cli_hexdump_rev(&(cmacvs_ctx.in_byte),1);
//		_delay_ms(500);

		cli_putstr_P(PSTR("\r\n starting last block"));
		cli_putstr_P(PSTR("\r\n\tlength       == "));
		cli_hexdump_rev(&mlen,4);
		cli_putstr_P(PSTR("\r\n\tbuffersize_B == "));
		cli_hexdump_rev(&(cmacvs_ctx.buffersize_B),2);
		uint16_t temp=(mlen-cmacvs_ctx.blocks*cmacvs_ctx.buffersize_B)*8;
		cli_putstr_P(PSTR("\r\n\t (temp)      == "));
		cli_hexdump_rev(&temp,2);
//		_delay_ms(500);
#endif
		uint16_t temp=(mlen-cmacvs_ctx.blocks*cmacvs_ctx.buffersize_B)*8;
		bcal_cmac_lastBlock( &(cmacvs_ctx.ctx), buffer, /* be aware of freaking compilers!!! */
//							length-(cmacvs_ctx.blocks)*((cmacvs_ctx.buffersize_B)*8));
		                    temp );
#if DEBUG
		cli_putstr_P(PSTR("\r\n starting ctx2cmac"));
		_delay_ms(500);
#endif
		bcal_cmac_ctx2mac(tag, tlen*8, &(cmacvs_ctx.ctx));
#if DEBUG
		cli_putstr_P(PSTR("\r\n starting cmac free"));
#endif
		bcal_cmac_free(&(cmacvs_ctx.ctx));
		cli_putstr_P(PSTR("\r\n Mac = "));
		cli_hexdump(tag, tlen);

	}
}


void cmacvs_test2(void){ /* Ver tests */
	int32_t klen, mlen, tlen;
	int32_t expect_input=0;

	if(!cmacvs_algo){
			cli_putstr_P(PSTR("\r\nERROR: select algorithm first!"));
		return;
	}
	char c;
	cmacvs_ctx.buffersize_B=pgm_read_word(&(cmacvs_algo->blocksize_b))/8;
	uint8_t tag[cmacvs_ctx.buffersize_B];
	uint8_t tag_ref[cmacvs_ctx.buffersize_B];
	uint8_t buffer[cmacvs_ctx.buffersize_B+5];
	cmacvs_ctx.buffer = buffer;
	cli_putstr_P(PSTR("\r\nbuffer_size = 0x"));
	cli_hexdump_rev(&(cmacvs_ctx.buffersize_B), 2);
	cli_putstr_P(PSTR(" bytes"));
	for(;;){
		cmacvs_ctx.blocks = 0;
		memset(buffer, 0, cmacvs_ctx.buffersize_B);
		klen = getValue_P(PSTR("Klen"));
		if(klen<0){
			return;
		}
		mlen = getValue_P(PSTR("Mlen"));
		if(mlen<0){
			return;
		}
		tlen = getValue_P(PSTR("Tlen"));
		if(tlen<0){
			return;
		}
		uint8_t key_buffer[klen];
#if DEBUG
		cli_putstr_P(PSTR("\r\nKLen == "));
		cli_hexdump_rev(&klen, 4);
		cli_putstr_P(PSTR("\r\nMLen == "));
		cli_hexdump_rev(&mlen, 4);
		cli_putstr_P(PSTR("\r\nTLen == "));
		cli_hexdump_rev(&tlen, 4);
#endif
		getKey(key_buffer, klen);
		if(mlen==0){
			expect_input=2;
		}else{
			expect_input=mlen*2;
		}
#if DEBUG
		cli_putstr_P(PSTR("\r\nexpected_input == "));
		cli_hexdump_rev(&expect_input, 4);
		if(expect_input==0)
			cli_putstr_P(PSTR("\r\nexpected_input == 0 !!!"));
#endif
		uint8_t ret;
#if DEBUG
		cli_putstr_P(PSTR("\r\n CMAC init"));
		cli_putstr_P(PSTR("\r\n (2) expected_input == "));
		cli_hexdump_rev(&expect_input, 4);
#endif
		ret = bcal_cmac_init(cmacvs_algo, key_buffer, klen*8, &(cmacvs_ctx.ctx));
		if(ret){
			cli_putstr_P(PSTR("\r\n bcal_cmac_init returned with: "));
			cli_hexdump(&ret, 1);
			return;
		}
#if DEBUG
		cli_putstr_P(PSTR("\r\n (3) expected_input == "));
		cli_hexdump_rev(&expect_input, 4);
		cli_putstr_P(PSTR("\r\n"));
#endif
		while((c=cli_getc_cecho())!='M' && c!='m'){
			if(!isspace(c)){
				cli_putstr_P(PSTR("\r\nERROR: wrong input (1) [0x"));
				cli_hexdump(&c, 1);
				cli_putstr_P(PSTR("]!\r\n"));
				bcal_cmac_free(&(cmacvs_ctx.ctx));
				return;
			}
		}
		if((c=cli_getc_cecho())!='s' && c!='S'){
				cli_putstr_P(PSTR("\r\nERROR: wrong input (2)!\r\n"));
				bcal_cmac_free(&(cmacvs_ctx.ctx));
				return;
		}
		if((c=cli_getc_cecho())!='g' && c!='G'){
				cli_putstr_P(PSTR("\r\nERROR: wrong input (3)!\r\n"));
				bcal_cmac_free(&(cmacvs_ctx.ctx));
				return;
		}
		while((c=cli_getc_cecho())!='='){
			if(!isspace(c)){
				cli_putstr_P(PSTR("\r\nERROR: wrong input (4)!\r\n"));
				bcal_cmac_free(&(cmacvs_ctx.ctx));
				return;
			}
		}
#if DEBUG
		cli_putstr_P(PSTR("\r\nparsing started"));
#endif
		cmacvs_ctx.buffer_idx = 0;
		cmacvs_ctx.in_byte    = 0;
		cmacvs_ctx.blocks     = 0;
		while(expect_input>0){
			c=cli_getc_cecho();
#if DEBUG
			cli_putstr_P(PSTR("\r\n\t("));
			cli_hexdump_rev(&expect_input, 4);
			cli_putstr_P(PSTR(") "));
			_delay_ms(500);
#endif
			if(buffer_add(c)==0){
				--expect_input;
			}else{
				if(!isblank((uint16_t)c)){
					cli_putstr_P(PSTR("\r\nERROR: wrong input (5) ("));
					cli_putc(c);
					cli_putstr_P(PSTR(")!\r\n"));
					bcal_cmac_free(&(cmacvs_ctx.ctx));
					return;
				}
			}
		}
#if DEBUG
		cli_putstr_P(PSTR("\r\nBuffer-A:"));
		cli_hexdump_block(buffer, cmacvs_ctx.buffersize_B, 5, 8);

		cli_putstr_P(PSTR("\r\n starting finalisation"));
		cli_putstr_P(PSTR("\r\n\tblocks     == "));
		cli_hexdump_rev(&(cmacvs_ctx.blocks),4);
		cli_putstr_P(PSTR("\r\n\tbuffer_idx == "));
		cli_hexdump_rev(&(cmacvs_ctx.buffer_idx),2);
		cli_putstr_P(PSTR("\r\n\tin_byte    == "));
		cli_hexdump_rev(&(cmacvs_ctx.in_byte),1);
//		_delay_ms(500);

		cli_putstr_P(PSTR("\r\n starting last block"));
		cli_putstr_P(PSTR("\r\n\tlength       == "));
		cli_hexdump_rev(&mlen,4);
		cli_putstr_P(PSTR("\r\n\tbuffersize_B == "));
		cli_hexdump_rev(&(cmacvs_ctx.buffersize_B),2);
		uint16_t temp=(mlen-cmacvs_ctx.blocks*cmacvs_ctx.buffersize_B)*8;
		cli_putstr_P(PSTR("\r\n\t (temp)      == "));
		cli_hexdump_rev(&temp,2);
//		_delay_ms(500);
#endif
		uint16_t temp=(mlen-cmacvs_ctx.blocks*cmacvs_ctx.buffersize_B)*8;
		bcal_cmac_lastBlock( &(cmacvs_ctx.ctx), buffer, /* be aware of freaking compilers!!! */
//							length-(cmacvs_ctx.blocks)*((cmacvs_ctx.buffersize_B)*8));
		                    temp );
#if DEBUG
		cli_putstr_P(PSTR("\r\n starting ctx2cmac"));
		_delay_ms(500);
#endif
		bcal_cmac_ctx2mac(tag, tlen*8, &(cmacvs_ctx.ctx));
#if DEBUG
		cli_putstr_P(PSTR("\r\n starting cmac free"));
#endif
		bcal_cmac_free(&(cmacvs_ctx.ctx));
		cli_putstr_P(PSTR("\r\n Mac = "));
		cli_hexdump(tag, tlen);
		getMac(tag_ref, tlen);
		if(memcmp(tag, tag_ref, tlen)){
			cli_putstr_P(PSTR("\r\n Result = F"));
		}else{
			cli_putstr_P(PSTR("\r\n Result = P"));
		}
	}
}
