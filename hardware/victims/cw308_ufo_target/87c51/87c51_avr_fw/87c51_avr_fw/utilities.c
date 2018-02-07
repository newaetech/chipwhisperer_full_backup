/*
 * utilities.c
 *
 * Created: 29/07/2016 4:21:20 PM
 *  Author: greg
 */ 

#include "utilities.h"
#include "uart.h"

#include <avr/io.h>
#include <avr/interrupt.h>

void utilities_init()
{
	// LED
	DDRA |= (1 << DDRA2);
	
	// Trigger
	DDRE |= (1 << DDRE3);
	
	// Also need to disable JTAG for this
	MCUCR |= (1 << JTD);
	MCUCR |= (1 << JTD);
}

void led2_high()
{
	PORTA |= (1 << PORTA2);
}

void led2_low()
{
	PORTA &= ~(1 << PORTA2);
}

void led2_toggle()
{
	PORTA ^= (1 << PORTA2);
}

ISR(TIMER1_OVF_vect)
{
	// Toggle LED3
	PORTF ^= (1 << PORTF0);
}

void heartbeat_enable()
{
	// Enable PF0 as output
	DDRF |= (1 << DDRF0);
	
	// Turn on timer 1 with 1/256 prescaling
	TCCR1B |= (1 << CS10) | (1 << CS11);
	
	// Enable OVF interrupts	
	TIMSK1 |= (1 << TOIE1);
}

void heartbeat_disable()
{
	// Disable OVF interrupts
	TIMSK1 &= ~(1 << TOIE1);
}

void trigger_high()
{
	PORTE |= (1 << PORTE3);
}

void trigger_low()
{
	PORTE &= ~(1 << PORTE3);
}

void target_enable()
{
	// Enable address pins
	DDRD = 0xFF;	// A0-7
	DDRF |= 0xC0;	// A8-9
	DDRA |= 0x03;	// A10-11
	DDRA |= 0x0C;	// A12-13
	// A14-15: not used

	// Turn on data pullups
	PORTC = 0xFF;
	
	// Enable control pins
	DDRF  |= (1 << 1) | (1 << 4) | (1 << 5);
	DDRA  |= (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7);
	
	// Set control levels (for verify)
	PORTF |=  (1 << 1);		// RST = 1
	PORTF |=  (1 << 4);		// P3.6 = 1
	PORTF |=  (1 << 5);		// P3.7 = 1
	PORTA |=  (1 << 7);		// ALE/PROG = 1
	PORTA &= ~(1 << 6);		// PSEN = 0;
	PORTA &= ~(1 << 4);		// P2.6 = 0
	// P2.7: controlled during verify operation
	// EA/VPP = 1: controlled by jumper
}