/* cli.h */
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

#ifndef CLI_H_
#define CLI_H_

#include <stdint.h>
#include <avr/pgmspace.h>

#ifndef VOID_FPT
#define VOID_FPT
typedef void(*void_fpt)(void);
#endif
typedef char (*cli_rx_fpt)(void);
typedef void (*cli_tx_fpt)(char);

#define CLI_BUFFER_BS 20
#define CMDLIST_ENTRY_SIZE 8

typedef struct {
	uint16_t   option_flags;
	PGM_VOID_P options[];
} cmdoption_t;

#define CLI_OPTION_DESC 0x01
#define CLI_OPTION_MANP 0x02

typedef struct {
	PGM_P        cmd_name;      /* string containing the function name */
	PGM_P        cmd_param_str; /* param descriptor string */
	void_fpt     cmd_function;  /* function pointer */
	cmdoption_t* options;
} cmdlist_entry_t;

extern cli_rx_fpt cli_rx;
extern cli_tx_fpt cli_tx;
extern uint8_t cli_echo;



void cli_putc(char c);
uint16_t cli_getc(void);
uint16_t cli_getc_cecho(void);
uint8_t cli_getsn(char* s, uint16_t n);
uint8_t cli_getsn_cecho(char* s, uint16_t n);
void cli_putstr(const char* s);
void cli_putstr_P(PGM_P s);
void cli_hexdump_byte(uint8_t byte);
void cli_hexdump(const void* data, uint16_t length);
void cli_hexdump_rev(const void* data, uint16_t length);
void cli_hexdump2(const void* data, uint16_t length);
void cli_hexdump_block(const void* data, uint16_t length, uint8_t indent, uint8_t width);

void echo_ctrl(char* s);
int8_t cmd_interface(PGM_VOID_P cmd_desc);


#endif /*CLI_H_*/
