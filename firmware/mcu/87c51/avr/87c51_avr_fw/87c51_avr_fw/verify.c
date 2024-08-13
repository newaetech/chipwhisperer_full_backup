/*
 * verify.c
 *
 * Created: 09/08/2016 11:53:19 AM
 *  Author: greg
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/delay_basic.h>
#include <stdint.h>

#include "utilities.h"

// Debug
#include "uart.h"

// Shared variables for data and address
uint16_t next_address;
uint8_t last_data;

void load_address(uint16_t address)
{
	// Move address onto A0-A13
	PORTD = (address & 0xFF);									// A0-A7
	PORTF = (PORTF & ~0xC0) | (((address >> 8) & 0x03) << 6);	// A8-A9
	PORTA = (PORTA & ~0x0F) | (((address >> 10) & 0x0F) << 0);	// A10-A13
}

uint8_t read_byte(uint16_t address)
{
	// Load address and enable
	load_address(address);
	PORTA &= ~(1 << 5);
	
	// Wait until data is valid
	_delay_loop_1(16);
	
	// Read byte off data bus
	uint8_t ret = PINC;
	
	// Disable and ensure that data is floating
	PORTA |= (1 << 5);
	_delay_loop_1(16);
	
	return ret;
}

// ISR to sync the read to the 1/6 clock
ISR(TIMER0_COMP_vect)
{
	// Disable additional timer interrupts
	TIMSK0 = 0;
	
	// Read the byte that we're interested in
	trigger_high();
	last_data = read_byte(next_address);
	trigger_low();
	
	// Put the 87C51 in a known state
	read_byte(0xFFFF);
}

void verify_init()
{
	// Start the timer
	TCCR0A = (1 << WGM01)   // CTC mode
	       | (1 << CS00);   // x1 clock
	OCR0A = 23;             // Overflow every nth clock cycle

	// Reset 8051
	DDRF |= (1 << DDRF1);
	PORTF |= (1 << PORTF1);
	_delay_ms(50);
	PORTF &= ~(1 << PORTF1);
	_delay_ms(50);
	
	// Sync to 1/6 clock
	// Make a lookup table with 6 entries - this is kinda dumb, but it should work
	// Need to do it this way so our sync operation is constant time
	uint8_t delay_lookup[64];
	delay_lookup[0b000011] = 0;
	delay_lookup[0b000110] = 5;
	delay_lookup[0b001100] = 4;
	delay_lookup[0b011000] = 3;
	delay_lookup[0b110000] = 2;
	delay_lookup[0b100001] = 1;

	// Get ready to store six bytes in an array
	uint8_t arr[6];
	// Time-sensitive: read PINA 6 times in 6 clock cycles
	// and put the 6 results into the array
	asm volatile(
	"in %[reg1],%[pina]\t\n"
	"in %[reg2],%[pina]\t\n"
	"in %[reg3],%[pina]\t\n"
	"in %[reg4],%[pina]\t\n"
	"in %[reg5],%[pina]\t\n"
	"in %[reg6],%[pina]\t\n"
	: [reg1] "=r" (arr[0]),
	  [reg2] "=r" (arr[1]),
	  [reg3] "=r" (arr[2]),
	  [reg4] "=r" (arr[3]),
	  [reg5] "=r" (arr[4]),
	  [reg6] "=r" (arr[5])
	: [pina] "I" (0) // PINA register is at 0x00
	);
	
	
	// Figure out which delay we are
	uint8_t samples = 0;
	for(uint8_t i = 0; i < 6; i++)
	{
		arr[i] = (arr[i] >> 7) & 0x01;
		samples |= (arr[i] << i);
	}
	
	// Sync up our timer
	TCNT0 = delay_lookup[samples];
}

uint8_t verify_byte(uint16_t address)
{	
	// Set up the ISR
	next_address = address;
	
	// Sync ourselves to the timer
	TIFR0 |= (1 << OCF0A);
	while(!(TIFR0 & (1 << OCF0A)));
	
	// Let the timer cause interrupts
	TIFR0 |= (1 << OCF0A);
	TIMSK0 = 0x02;
			
	// Wait for the ISR to get a return value
	// Also, make sure we only use instructions that take 1 cycle...
	asm volatile(
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
	);
	
	// The data byte is in the shared variable
	return last_data;
}