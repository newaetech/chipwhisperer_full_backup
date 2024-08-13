/* uart_i-asm.S */
/*
    This file is part of the AVR-uart_i.
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
/**
 * \file     uart_i-asm.S
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-07-24
 * \license  GPLv3 or later
 * \ingroup  uart_i
 * \brief    implementation of interrupt based uart
 */

#include <avr/io.h>
#include "config.h"
#include "avr-asm-macros.S"
#include "uart_defs.h"

#define XON_VALUE  0x11
#define XOFF_VALUE 0x13

#if UART0_I

#ifndef UART0_PARATY
# warning "UART0: using default paraty: 'none'"
# define UART0_PARATY UART_PARATY_NONE
#endif

#ifndef UART0_STOPBITS
# warning "UART0: using default ammount of stop bits: '1'"
# define UART0_STOPBITS UART_STOPBITS_1
#endif

#ifndef UART0_DATABITS
# warning "UART0: using default ammount of data bits: '8'"
# define UART0_DATABITS UART_DATABITS_8
#endif

#ifdef UDR
# define OLD_UART
# ifdef UDR0
#  error "can not decide which registernames to use, UDR and UDR0 are defined"
# endif
#endif

#ifdef OLD_UART
# define UCSR0A UCSRA
# define UCSR0B UCSRB
# define UCSR0C UCSRC
# define UBRR0H UBRRH
# define UBRR0L UBRRL
# define UDR0   UDR
# define TXEN0  TXEN
# define RXEN0  RXEN
# define UDRE0  UDRE
# define RXC0   RXC
# define TXB80  TXB8
# define RXB80  RXB8
# define U2X0   U2X
# define UDRIE0 UDRIE
# define RXCIE0 RXCIE
#endif

#ifdef USART0_RX_vect
# define RX_ISR USART0_RX_vect
#endif

#ifdef USART_RXC_vect
# define RX_ISR USART_RXC_vect
#endif

#ifdef USART0_UDRE_vect
# define TX_ISR USART0_UDRE_vect
#endif

#ifdef USART_UDRE_vect
# define TX_ISR USART_UDRE_vect
#endif

#define CBB_SIZE 10

#define UART0_CBB_RX_OFFSET  0
#define UART0_CBB_TX_OFFSET 10

#define CTX_BASE_SIZE (2*(CBB_SIZE))

#if UART0_HOOK
#  if UART0_SWFLOWCTRL
#    define UART0_CTX_SIZE (3+2+CTX_BASE_SIZE)
#    define UART0_HOOK_OFFSET  20
#    define UART0_HOOKR_OFFSET 22
#    define UART0_TXON_OFFSET  23
#    define UART0_RXON_OFFSET  24
#  else
#    define UART0_CTX_SIZE (3+0+CTX_BASE_SIZE)
#    define UART0_HOOK_OFFSET  20
#    define UART0_HOOKR_OFFSET 22
#  endif
#else
#  if UART0_SWFLOWCTRL
#    define UART0_CTX_SIZE (0+2+CTX_BASE_SIZE)
#    define UART0_TXON_OFFSET  20
#    define UART0_RXON_OFFSET  21
#  else
#    define UART0_CTX_SIZE (0+0+CTX_BASE_SIZE)
#  endif
#endif

	.section .bss
.global uart0_rxbuffer
uart0_rxbuffer:
	.fill UART0_RXBUFFER_SIZE, 1, 0
.global uart0_txbuffer
uart0_txbuffer:
	.fill UART0_TXBUFFER_SIZE, 1, 0
.global uart0_ctx
uart0_ctx:
	.fill UART0_CTX_SIZE, 1, 0	
/******************************************************************************/	
/* Baudrate calculation */
#ifdef BAUD
#undef BAUD
#endif

#define BAUD UART0_BAUD_RATE
#include "setbaud_asm.inc"

	.section .text
/******************************************************************************/	
/*
 *	void uart0_init(void){
 *		circularbytebuffer_init2(UART0_RXBUFFER_SIZE, &(uart0_ctx.rxb), uart0_rxbuffer);
 *		circularbytebuffer_init2(UART0_TXBUFFER_SIZE, &(uart0_ctx.txb), uart0_txbuffer);
 *	#if UART0_HOOK
 *		uart0_ctx.hook = NULL;
 *		uart0_ctx.hook_running = 0;
 *	#endif
 *	#if UART0_SWFLOWCTRL
 *		uart0_ctx.txon = 1;
 *		uart0_ctx.rxon = 1;
 *	#endif
 *		#define BAUD UART0_BAUD_RATE
 *		#include <util/setbaud.h>	
 *		UBRR0H = UBRRH_VALUE;
 *		UBRR0L = UBRRL_VALUE;
 *		#if USE_2X
 *		UCSR0A |= _BV(U2X0);
 *		#else
 *		UCSR0A &= ~_BV(U2X0);
 *		#endif
 *		UCSR0C = (UART0_PARATY<<4)|(UART0_STOPBITS<<3)|((UART0_DATABITS&3)<<1);
 *		UCSR0B = _BV(RXCIE0) | _BV(UDRIE0) | _BV(RXEN0) | _BV(TXEN0) ; / * enable TX and RX and interrupts * /
 *		sei();
 *	}
 *
 */
.global uart0_init
uart0_init:
	ldi r24, UART0_RXBUFFER_SIZE
	clr r25
	ldi r22, lo8(uart0_ctx+UART0_CBB_RX_OFFSET)
	ldi r23, hi8(uart0_ctx+UART0_CBB_RX_OFFSET)
	ldi r20, lo8(uart0_rxbuffer)
	ldi r21, hi8(uart0_rxbuffer)
	rcall circularbytebuffer_init2
	ldi r24, UART0_TXBUFFER_SIZE
	clr r25
	ldi r22, lo8(uart0_ctx+UART0_CBB_TX_OFFSET)
	ldi r23, hi8(uart0_ctx+UART0_CBB_TX_OFFSET)
	ldi r20, lo8(uart0_txbuffer)
	ldi r21, hi8(uart0_txbuffer)
	rcall circularbytebuffer_init2
#if UART0_SWFLOWCTRL
	ldi r30, lo8(uart0_ctx)
	ldi r31, hi8(uart0_ctx)
	ldi r24, 1
	std Z+UART0_TXON_OFFSET, r24
	std Z+UART0_RXON_OFFSET, r24
#endif	
	ldi r24, UBRRH_VALUE
	STORE_IO UBRR0H, r24
	ldi r24, UBRRL_VALUE
	STORE_IO UBRR0L, r24
#if USE_2X
	SET_BIT_IO UCSR0A, U2X0, r24
#else
  	CLEAR_BIT_IO UCSR0A, U2X0, r24
#endif
	ldi r24, (UART0_PARATY<<4)|(UART0_STOPBITS<<3)|((UART0_DATABITS&3)<<1)
	STORE_IO UCSR0C, r24
	ldi r24, _BV(RXCIE0) | _BV(UDRIE0) | _BV(RXEN0) | _BV(TXEN0)  
	STORE_IO UCSR0B, r24
	sei
	ret
	
/******************************************************************************/	
/*
 * 	ISR(USART0_UDRE_vect){
 *		uint16_t x;
 *		x = circularbytebuffer_get_fifo(&(uart0_ctx.txb));
 *		if(x==0xffff){
 *			/ * the transmit buffer is empty, disable interrupt * /
 *			UCSR0B &= (uint8_t)~_BV(UDRIE0);
 *			return;
 *		}
 *	#if UART0_SWFLOWCTRL
 *		while(!uart0_ctx.txon)
 *			;
 *	#endif		
 *		UDR0 = x;
 *	}
 */ 
 
.global TX_ISR
TX_ISR:
	push r1
	push r21
	push r22
	in r21, _SFR_IO_ADDR(SREG)
	CLEAR_BIT_IO UCSR0B, UDRIE0, r22
	sei
	push_range 23, 27
	push_range 30, 31
	clr r1
	ldi r24, lo8(uart0_ctx+UART0_CBB_TX_OFFSET)
	ldi r25, hi8(uart0_ctx+UART0_CBB_TX_OFFSET)
	rcall circularbytebuffer_get_fifo
	cpi r25, 0xff
	brne 20f
	CLEAR_BIT_IO UCSR0B, UDRIE0, r24
	rjmp 99f
20:
#if UART0_SWFLOWCTRL
	ldi r30, lo8(uart0_ctx+UART0_TXON_OFFSET)
	ldi r31, hi8(uart0_ctx+UART0_TXON_OFFSET)
30:
	ld r22, Z
	tst r22
	breq 30b
#endif
	STORE_IO UDR0, r24
	SET_BIT_IO UCSR0B, UDRIE0, r22
99:
	ori r21, 0x80 /* set I bit */
	out _SFR_IO_ADDR(SREG), r21
	pop_range 30, 31
	pop_range 21, 27
	pop r1
	ret
	
/******************************************************************************/
/*
 *	void uart0_putc (uint16_t c){
 *	#if UART0_SWFLOWCTRL
 *		while(!uart0_ctx.txon)
 *			;
 *	#endif	
 *		while(circularbytebuffer_cnt(&(uart0_ctx.txb))==UART0_TXBUFFER_SIZE)
 *			;
 *		cli();		
 *		circularbytebuffer_append((uint8_t)c, &(uart0_ctx.txb));
 *		sei();
 *		UCSR0B |= (uint8_t)_BV(UDRIE0);
 *	}
 *
 * param c:  r24:r25
 */
.global uart0_putc
uart0_putc:
	mov r18, r24
#if UART0_SWFLOWCTRL
	ldi r30, lo8(uart0_ctx+UART0_TXON_OFFSET)
	ldi r31, hi8(uart0_ctx+UART0_TXON_OFFSET)
10:
	ld r22, Z
	tst r22
	breq 10b
#endif
	ldi r26, lo8(uart0_ctx+UART0_CBB_TX_OFFSET)
	ldi r27, hi8(uart0_ctx+UART0_CBB_TX_OFFSET)
20:
	movw r24, r26
	cli
	rcall circularbytebuffer_cnt
	sei
	cpi r24, UART0_TXBUFFER_SIZE
	breq 20b
	movw r22, r26
	mov r24, r18
	clr r25
	cli
	rcall circularbytebuffer_append
	SET_BIT_IO UCSR0B, UDRIE0, r24
	reti

/******************************************************************************/
/*
 *	ISR(USART0_RX_vect){
 *		uint16_t c;
 *		c = UDR0;
 *	#if UART0_SWFLOWCTRL
 *		if(c==XON_VALUE){
 *			uart0_ctx.txon = 1;
 *			return;
 *		}
 *		if(c==XOFF_VALUE){
 *			uart0_ctx.txon = 0;
 *			return;
 *		}
 *	#endif		
 *	#if	UART0_HOOK
 *		if((!uart0_ctx.hook_running) && uart0_ctx.hook){
 *			uart0_ctx.hook_running=1;
 *			sei();
 *			do{
 *				uart0_ctx.hook(c);
 *			}while((c=circularbytebuffer_get_fifo(&(uart0_ctx.rxb)))!=0xffff);
 *			uart0_ctx.hook_running=0;
 *			return;
 *		}
 *	#endif
 *		if(circularbytebuffer_cnt(&(uart0_ctx.rxb))==UART0_RXBUFFER_SIZE)
 *			return;
 *		circularbytebuffer_append(c, &(uart0_ctx.rxb));
 *	#if UART0_SWFLOWCTRL
 *		if(circularbytebuffer_cnt(&(uart0_ctx.rxb))>UART0_THRESH_HIGH && uart0_ctx.rxon){
 *			uart0_ctx.rxon = 0;
 *			circularbytebuffer_push(XOFF_VALUE, &(uart0_ctx.txb));
 *			UCSR0B |= (uint8_t)_BV(UDRIE0);
 *		}
 *		if(circularbytebuffer_cnt(&(uart0_ctx.rxb))<UART0_THRESH_LOW && !uart0_ctx.rxon){
 *			uart0_ctx.rxon = 1;
 *			circularbytebuffer_push(XON_VALUE, &(uart0_ctx.txb));
 *			UCSR0B |= (uint8_t)_BV(UDRIE0);
 *		}
 *	#endif		
 *	}
 *
 */
 .global RX_ISR
 RX_ISR:
 	push_range 0, 1
	push_range 16, 31
	in r16, _SFR_IO_ADDR(SREG)
	clr r1
	LOAD_IO r24, UDR0
#if UART0_SWFLOWCTRL
    ldi r26, lo8(uart0_ctx+UART0_TXON_OFFSET)
	ldi r27, hi8(uart0_ctx+UART0_TXON_OFFSET)
	cpi r24, XON_VALUE
	breq 11f
	cpi r24, XOFF_VALUE
	brne 12f
	clr r24
11:	st X, r24
	rjmp 99f
12:
	push r24
/* now the "sending" part*/
	ldi r24, lo8(uart0_ctx+UART0_CBB_RX_OFFSET)
	ldi r25, hi8(uart0_ctx+UART0_CBB_RX_OFFSET)
	rcall circularbytebuffer_cnt
	ldi r30, lo8(uart0_ctx+UART0_RXON_OFFSET)
	ldi r31, hi8(uart0_ctx+UART0_RXON_OFFSET)
	ld r18, Z
	tst r18
	breq 15f/* branch if rxon inactive -> we had send an XOFF earlier */
	/* ok, we did not have send an XOFF, should we? */
	cpi r24, UART0_THRESH_HIGH
	brlo 90f /* ok, nothing critical, go on */
	st Z, r1
	ldi r24, XOFF_VALUE
;	sbi _SFR_IO_ADDR(PORTD), 5
	rjmp 16f
15:
	cpi r24, UART0_THRESH_LOW
	brsh 90f /* nothing has changed */
	/* if we get here, we had send an XOFF and are now below threshold */
	/* so we sen an XON */
	ldi r24, XON_VALUE
	cbi _SFR_IO_ADDR(PORTD), 5
	st Z, r24
16:
	ldi r22, lo8(uart0_ctx+UART0_CBB_TX_OFFSET)
	ldi r23, hi8(uart0_ctx+UART0_CBB_TX_OFFSET)
	rcall circularbytebuffer_push
	SET_BIT_IO UCSR0B, UDRIE0, r24
90:
	pop r24
#endif /* UART0_SWFLOWCTRL */	
20:	
#if UART0_HOOK
	ldi r30, lo8(uart0_ctx)
	ldi r31, hi8(uart0_ctx)
	ldd r22, Z+UART0_HOOKR_OFFSET
	tst r22
	brne 50f
	ldd r26, Z+UART0_HOOK_OFFSET
	ldd r27, Z+UART0_HOOK_OFFSET+1
	adiw r26, 0
	breq 50f
	movw r28, r26
	movw r16, r30
	sei
30:
	/* now we can run the hook */
	movw r30, r28
	clr r25
	icall
	movw r24, r16
	rcall circularbytebuffer_get_fifo 
	cpi r25, 0xff
	brne 30b
	clr r24
	st -Y, r24 /* write 0 to uart0_hook_running */
	rjmp 99f
#endif /* UART0_HOOK */
50:
	ldi r22, lo8(uart0_ctx+UART0_CBB_RX_OFFSET)
	ldi r23, hi8(uart0_ctx+UART0_CBB_RX_OFFSET)
	clr r25
;	sbi _SFR_IO_ADDR(PORTD), 6
	rcall circularbytebuffer_append
99:	
	out _SFR_IO_ADDR(SREG), r16
	pop_range 16, 31
	pop_range 0, 1
	reti

/******************************************************************************/
/*
 *	uint16_t uart0_getc(void){
 *		uint8_t ret;
 *		while(circularbytebuffer_cnt(&(uart0_ctx.rxb))==0)
 *			;
 *		cli();	
 *		ret = circularbytebuffer_get_fifo(&(uart0_ctx.rxb));
 *		sei();
 *		return 	ret;
 *	}
 */
 .global uart0_getc
 uart0_getc:
 	ldi r22, lo8(uart0_ctx+UART0_CBB_RX_OFFSET)
 	ldi r23, hi8(uart0_ctx+UART0_CBB_RX_OFFSET)
 10:
 	movw r24, r22
	rcall circularbytebuffer_cnt
	tst r24
	breq 10b
	movw r24, r22
	cli
	rcall circularbytebuffer_get_fifo
	clr r25
	reti
	
/******************************************************************************/
/*
 *	uint8_t uart0_dataavail(void){
 *		return circularbytebuffer_cnt(&(uart0_ctx.rxb));
 *	}
 */
.global uart0_dataavail
uart0_dataavail:
 	ldi r24, lo8(uart0_ctx+UART0_CBB_RX_OFFSET)
 	ldi r25, hi8(uart0_ctx+UART0_CBB_RX_OFFSET)
 	rjmp circularbytebuffer_cnt
	
/******************************************************************************/
#if	UART0_HOOK
/*
 *	void uart0_sethook(void(*fpt)(uint8_t)){
 *		uart0_ctx.hook = fpt;
 *	}
 */
.global uart0_sethook
uart0_sethook:
 	ldi r26, lo8(uart0_ctx+UART0_HOOK_OFFSET)
 	ldi r27, hi8(uart0_ctx+UART0_HOOK_OFFSET)
 	st X+, r24
	st X+, r25
	ret
#endif
	
#endif /* UART0_I */
