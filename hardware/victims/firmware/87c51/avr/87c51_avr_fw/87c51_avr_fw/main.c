/*
 * main.c
 *
 * Created: 29/07/2016 2:58:13 PM
 * Author : greg
 */ 

#define F_CPU 7384615
//#define F_CPU 4000000
#define UART_BAUD 4800

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay_basic.h>
#include <util/delay.h>
#include <stdio.h>

#include "uart.h"
#include "utilities.h"
#include "simpleserial.h"
#include "verify.h"


void sleep_avr()
{
	// Go to sleep
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_cpu();
}

int main(void)
{
	// First, check if we're supposed to be asleep
	uint8_t mode_verify = PINE & (1 << PINE2);
	if(!mode_verify)
	{
		sleep_avr();
	}
	
	// Then, we can set up everything else
	utilities_init();
	uart_init(F_CPU/16/UART_BAUD - 1);
	verify_init();
	
	led2_low();
	trigger_low();
		
	target_enable();
	verify_byte(0xFFFF);
	
	sei();

    while (1) 
    {
		// Let the user know we're waiting
		heartbeat_enable();
		
		if(simpleserial_get(2))
		{	
			heartbeat_disable();
			uint8_t* address = simpleserial_getaddress();
			
			uint16_t addr_16 = (address[0] << 8) | (address[1]);
			
			// Verify code at this address
			uint8_t response[1];
			response[0] = verify_byte(addr_16);
			
			simpleserial_put(response, 1);	
		}
    }
}

