/* cli-core.S */
/*
    This file is part of the AVR-Huffman.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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

#include "avr-asm-macros.S"

#define CMDLIST_ENTRY_SIZE 8

/******************************************************************************/
/* uint16_t max_cmd_length(PGM_VOID_P cmdlist){
	uint16_t t,ret=0;
	char* str;
	for(;;){
		str = (char*)pgm_read_word(cmdlist);
		cmdlist = (uint8_t*)cmdlist + CMDLIST_ENTRY_SIZE;
		if(str==NULL)
			return ret;
		t = strlen_P(str);
		if(t>ret)
			ret=t;
	}
}
 *    param cmdlist:  r24:r25
 */
TMP_CNT_0 = 26
TMP_CNT_1 = 27

.global max_cmd_length
max_cmd_length:
	movw r30, r24
	clr r24
	clr r25
1:
	lpm r26, Z+
	lpm r27, Z+
	movw r20, r30
	adiw r26, 0
	brne 3f
	ret
3:
	movw r30, r26
	clr TMP_CNT_0
	clr TMP_CNT_1
4:
	lpm r1, Z+
	adiw TMP_CNT_0, 1
	tst r1
	brne 4b
	sbiw TMP_CNT_0, 1
	movw r30, r20
	adiw r30, CMDLIST_ENTRY_SIZE-2
	cp  r24, TMP_CNT_0
	cpc r25, TMP_CNT_1
	brge 1b
	movw r24, TMP_CNT_0
	rjmp 1b

/******************************************************************************/
.global echo_ctrl
echo_ctrl:
	call strstrip
	tst r24
	brne 3f
	tst r25
	breq print_echo_status
3:
	call strlwr
	movw r26, r24
	ld r20, X+
	tst r20
	breq print_echo_status
	cpi r20, '1'
	breq exit_1
	cpi r20, '0'
	breq exit_0
	cpi r20, 't'
	breq exit_1
	cpi r20, 'f'
	breq exit_0
	ld r20, X+
	cpi r20, 'n'
	breq exit_1
	cpi r20, 'f'
	breq exit_0
	ret
exit_1:
	ldi r20, 1
	rjmp exit
exit_0:
	ldi r20, 0
exit:
	sts cli_echo, r20
	ret
print_echo_status:
	ldi r24, lo8(pecho)
	ldi r25, hi8(pecho)
	rcall cli_putstr_P
	lds r20, cli_echo
	tst r20
	breq 3f
	clr r25
	ldi r24, 'n'
	rcall cli_putc
	rjmp 4f
3:	clr r25
	ldi r24, 'f'
	rcall cli_putc
	clr r25
	ldi r24, 'f'
	rcall cli_putc
4:	clr r25
	ldi r24, '\r'
	rcall cli_putc
	clr r25
	ldi r24, '\n'
	rcall cli_putc
	ret
pecho:
.asciz "\r\necho is o"

/******************************************************************************/
/* int8_t search_and_call(char* cmd, uint16_t maxcmdlength, PGM_VOID_P cmdlist)
 *    param cmd:          r24:r25
 *    param maxcmdlength: r22:r23
 *    param cmdlist:      r20:r21
 */
LEN_0 = 14
LEN_1 = 15
CMD_0 = 16
CMD_1 = 17

snc_auto_help:
	movw r24, r22
	movw r22, r20
	rcall cli_auto_help
snc_return_1:
	ldi r24, 1
	rjmp snc_return
snc_return_2:
	ldi r24, 2
snc_return:
	clr r25
	ret

.global search_and_call
search_and_call:
	adiw r24, 0
	breq snc_return_1
1:	movw r26, r24
	ld r18, X+
	cpi r18, 0
	breq snc_return_1
	cpi r18, '#'
	breq snc_return_1
	cpi r18, '?'
	breq 2f
	cpi r18, 'h'
	brne 1f
	ld r18, X+
	cpi r18, 'e'
	brne 1f
	ld r18, X+
	cpi r18, 'l'
	brne 1f
	ld r18, X+
	cpi r18, 'p'
	brne 1f
	ld r18, X+
	tst r18
	brne 1f
2:	rjmp snc_auto_help
1:
	push r28
	push r29
	push CMD_0
	push CMD_1
	push LEN_0
	push LEN_1
	movw CMD_0, r24
	movw r28, r20
	rcall firstword_length
	movw LEN_0, r24
	movw r26, CMD_0
	add r26, LEN_0
	adc r27, LEN_1
	st X, r1
	movw r24, CMD_0
	ldi r22, lo8(exit_str)
	ldi r23, hi8(exit_str)
	call strcmp_P
	adiw r24, 0
	breq snc_return_0
1:
	movw r30, r28
	lpm r24, Z+
	lpm r25, Z+
	adiw r24, 0
	brne 3f
	ldi r24, lo8(nocommand_str)
	ldi r25, hi8(nocommand_str)
	rcall cli_putstr_P
	ldi r24, 1
	rjmp 5f
3:	movw r22, r24
	movw r24, CMD_0
	call strcmp_P
	adiw r24, 0
	breq snc_found
	adiw r28, CMDLIST_ENTRY_SIZE
	rjmp 1b
snc_found:
	movw r24, CMD_0
	add r24, LEN_0
	adc r25, LEN_1
	adiw r24, 1
	movw r30, r28
	adiw r30, 4
	lpm r22, Z+
	lpm r23, Z+
	movw r30, r22
	adiw r30, 0
	breq 4f ;snc_return_2
	icall
	ldi r24, 1
	rjmp 5f
4:	ldi r24, 2
snc_return_0:
	clr r24
5:	pop LEN_1
	pop LEN_0
	pop CMD_1
	pop CMD_0
	pop r29
	pop r28
	rjmp snc_return

exit_str:
.asciz "exit"
help_str:
.asciz "help"
nocommand_str:
.asciz "\r\ncommand not found, try 'help'"

/******************************************************************************/
/* void cli_option_listing(char* buffer, PGM_VOID_P cmdlist)
 *   param buffer:    r24:r25
 *   param cmdlist:   r22:r23
 */
ITM_0 = 12
ITM_1 = 13
CMD_0 = 14
CMD_1 = 15
LEN_0 = 16
LEN_1 = 17
LST_0 = 28
LST_1 = 29
.global cli_option_listing
cli_option_listing:
	push_range 12, 17
	push_range 28, 29
	movw CMD_0, r24
	movw LST_0, r22
	call strlen
	sbiw r24, 1
	movw LEN_0, r24
	brpl 1f
	clr LEN_0
	clr LEN_1
1:
	movw r30, LST_0
	lpm r24, Z+
	lpm r25, Z+
	movw ITM_0, r24
	adiw r24, 0
	breq 99f
	movw r22, r24
	movw r24, CMD_0
	movw r20, LEN_0
	adiw r28, CMDLIST_ENTRY_SIZE
	call strncmp_P
	adiw r24, 0
	brne 1b
	ldi r24, lo8(itemprefix_str)
	ldi r25, hi8(itemprefix_str)
	rcall cli_putstr_P
	movw r24, ITM_0
	rcall cli_putstr_P
	rjmp 1b
99:
	ldi r24, lo8(prompt_str)
	ldi r25, hi8(prompt_str)
	rcall cli_putstr_P
	movw r24, CMD_0
	rcall cli_putstr
	pop_range 28, 29
	pop_range 12, 17
	ret

prompt_str:
.asciz "\r\n>"
itemprefix_str:
.asciz "\r\n   "

/******************************************************************************/
/* void cli_auto_help(uint16_t maxcmdlength, PGM_VOID_P cmdlist)
 *   param maxcmdlength:  r24:r25
 *   param cmdlist:       r22:r23
 */

TMP_0 = 14
TMP_1 = 15
LEN_0 = 16
LEN_1 = 17
LST_0 = 28
LST_1 = 29
cli_auto_help:
	push_range 16, 17
	push_range 28, 29
	movw LEN_0, r24
	movw r28, r22
	ldi r24, lo8(ahelphead_str)
	ldi r25, hi8(ahelphead_str)
	rcall cli_putstr_P
1:
	movw r30, r28
	lpm r24, Z+
	lpm r25, Z+
	movw r28, r30
	adiw r24, 0
	breq 99f
	movw TMP_0, r24
	rcall cli_putstr_P
	movw r24, TMP_0
	call strlen_PF
	movw TMP_0, LEN_0
	sub TMP_0, r24
	sbc TMP_1, r25
	breq 4f
2:	ldi r24, ' '
	clr r25
	rcall cli_putc
	dec TMP_0
	brne 2b
4:	ldi r24, lo8(sep_str)
	ldi r25, hi8(sep_str)
	rcall cli_putstr_P
	ldi r22, lo8(none_str)
	ldi r23, hi8(none_str)
	movw r30, r28
	lpm r24, Z+
	lpm r25, Z+
	movw r28, r30
	adiw r24, 0
	breq 5f
	ldi r22, lo8(yes_str)
	ldi r23, hi8(yes_str)
5:	movw r24, r22
	rcall cli_putstr_P
	ldi r24, lo8(sep2_str)
	ldi r25, hi8(sep2_str)
	rcall cli_putstr_P
	movw r30, r28
	adiw r30, 1
	lpm r24, Z
	rcall cli_hexdump_byte
	movw r30, r28
	lpm r24, Z
	rcall cli_hexdump_byte
	ldi r24, '\r'
	clr r25
	rcall cli_putc
	ldi r24, '\n'
	clr r25
	rcall cli_putc
	adiw r28, CMDLIST_ENTRY_SIZE-4
	rjmp 1b
99:
	pop_range 28, 29
	pop_range 14, 17
	ret
ahelphead_str:
.asciz "\r\n[auto help] available commands:\r\n <command> - <params> - <address>\r\n"
sep_str:
.asciz " - "
none_str:
.asciz "none"
yes_str:
.asciz "yes "
sep2_str:
.asciz" \t - 0x"
.byte 0




