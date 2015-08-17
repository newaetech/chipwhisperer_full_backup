/* cli.c */
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
/**
 *
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3 or later
 *
 * components to help implementing simple command based interaction
 *
 **/

#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "string-extras.h"
#include "cli.h"
#include "config.h"
#include "hexdigit_tab.h"

cli_rx_fpt cli_rx = NULL;
cli_tx_fpt cli_tx = NULL;
uint8_t    cli_echo = 1;

void     echo_ctrl(char* s);
uint16_t max_cmd_length(PGM_VOID_P cmdlist);
int8_t   search_and_call(char* cmd, uint16_t maxcmdlength, PGM_VOID_P cmdlist);
void     cli_option_listing(char* buffer, PGM_VOID_P cmdlist);
void     cli_auto_help(uint16_t maxcmdlength, PGM_VOID_P cmdlist);

typedef void(*str_fpt)(char*);
#define CLI_ENTER     13
#define CLI_BACKSPACE  8
#define CLI_TABULATOR  9

uint8_t cli_completion(char* buffer, uint16_t maxcmdlength, PGM_VOID_P cmdlist){
	uint8_t i=0;
	char ref[maxcmdlength+1];
	char* itemstr;
	ref[0]='\0';
	/* check if we are behind the first word */
	while(buffer[i]){
		if(!isgraph(buffer[i++]))
			return 0;
	}
	for(;;){
		itemstr = (char*)pgm_read_word(cmdlist);
		if(itemstr==NULL)
			break;
		cmdlist = (uint8_t*)cmdlist +CMDLIST_ENTRY_SIZE;
		if(!strncmp_P(buffer, itemstr, i)){
			if(!ref[0]){
				strcpy_P(ref, itemstr);
			}else{
				ref[stridentcnt_P(ref, itemstr)]='\0';
			}
		}
	}
	i = strcmp(buffer, ref);
	if(i)
		strcpy(buffer, ref);
	return ~i;
}

int8_t cmd_interface(PGM_VOID_P cmd_desc){
	uint16_t cli_buffer_size;
	uint16_t cli_buffer_index;
	int8_t exit_code;
	uint8_t completion_failed=0;
	char* cli_buffer;
	char c;
	uint16_t maxcmdlength = max_cmd_length(cmd_desc);
	cli_buffer = calloc(1,cli_buffer_size=maxcmdlength+2);
	cli_buffer_index=0;
	if(!cli_rx)
		return -1;
	if(cli_tx)
		cli_tx('>');
	for(;;){
		c = cli_rx();
		switch (c){
		case CLI_ENTER:
			if((exit_code=search_and_call(cli_buffer, maxcmdlength, cmd_desc))<=0){
				free(cli_buffer);
				return exit_code;
			}
			/* cli_putstr(cli_buffer); */

			memset(cli_buffer, 0, cli_buffer_size);
			cli_buffer_index=0;
			/* cli_putstr_P(PSTR(" DONE\r\n>")); */
			cli_putstr_P(PSTR("\r\n>"));
			completion_failed=0;
			break;
		case CLI_BACKSPACE:
			completion_failed=0;
			if(cli_buffer_index==0)
				break;
			cli_buffer_index--;
			cli_buffer[cli_buffer_index] = '\0';
			if(cli_echo && cli_tx){
				cli_tx(c);
			}
			break;
		case CLI_TABULATOR:
			if(completion_failed || cli_buffer_index==0){
				if(cli_tx)
					cli_option_listing(cli_buffer, cmd_desc);
			} else {
				uint16_t old_idx = cli_buffer_index;
				completion_failed =
					~cli_completion(cli_buffer, maxcmdlength, cmd_desc);
				cli_buffer_index = strlen(cli_buffer);
				if(cli_echo && cli_tx){
					while(old_idx<cli_buffer_index){
						cli_tx(cli_buffer[old_idx++]);
					}
				}
			}
			break;
		default:
			completion_failed=0;
			if(cli_echo && cli_tx){
				cli_tx(c);
			}
			if(cli_buffer_index+1==cli_buffer_size){
				cli_buffer = realloc(cli_buffer, cli_buffer_size+=CLI_BUFFER_BS);
				if(!cli_buffer){
					return -2;
				}
				memset(cli_buffer+cli_buffer_index+1, 0, CLI_BUFFER_BS);
			}
			cli_buffer[cli_buffer_index++] = c;
		}
	}
}
