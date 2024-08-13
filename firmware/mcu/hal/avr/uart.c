/* Simple routine to use the hardware UART. Designed to NOT use interrupts,
this code is extreamly portable, and also extreamly simple.

Created by: Colin O'Flynn
Contact: c_oflynn@yahoo.com or coflynn@newae.com or username c_oflynn on
www.avrfreaks.net

These routines are released free of restrictions, but if anything bad happens
(including but not limited to loss of your time, loss of profit, loss of life,
injury, loss of money, loss of your dog) it is your OWN fault, NO ONE else
can be held responsible*/

#include <avr/io.h>
#include "uart.h"

#define BAUD_RATE0_REG  (unsigned int)(CPU_CLK_SPEED / (16 * BAUD_RATE0) ) - 1
//Actual baud rate, can be used to calculate error
#define ACTUAL_BAUD0    (unsigned int)(CPU_CLK_SPEED / (16 * BAUD_RATE0_REG + 1)

#define BAUD_RATE1_REG  (unsigned int)(CPU_CLK_SPEED / (16 * BAUD_RATE1) ) - 1
//Actual baud rate, can be used to calculate error
#define ACTUAL_BAUD1    (unsigned int)(CPU_CLK_SPEED / (16 * BAUD_RATE1_REG + 1)


/*if you want to be able to change the baud rate "on the fly", just add in some
code that calculates the proper baud rate register settings. The calculations
are used above with #define BAUD_RATE0_REG. However, if you want to change
baud rates BUT will only need a few, it would be easier to pre-calculate
the baud register settings in pre-processor, and use a switch statement that
lets you select between a few baud rates*/

void init_uart0(void)
{
	//turn on TX and RX
   RXTXEN0_REG = (1<<RX0EN) | (1<<TX0EN);

   //set up baud rate
#if (BAUDREGS == 2)
   	BAUD0H_REG = (unsigned char)(BAUD_RATE0_REG >> 8);
   	BAUD0L_REG = (unsigned char)BAUD_RATE0_REG;
   #else
   	BAUD0L_REG = (unsigned char)BAUD_RATE0_REG;
#endif
   return;
}

unsigned char input_ch_w_timeout_0(char *data, volatile unsigned int timeout)
{
   unsigned int	timeout_counter = 0;

   //check if a byte has been recieved or if the timeout has been excedded
   while (timeout_counter != timeout) {
		if ((STAT0RXTX_REG & (1<<RX0C)) == (1<<RX0C)) {
			*data = UDR0;
			return BYTE_REC;
		}
		timeout_counter++;
	}

	return TIMEOUT;
}

char input_ch_0(void)
{
   //check if a byte has been recieved or if the timeout has been excedded
    while ((STAT0RXTX_REG & (1<<RX0C)) != (1<<RX0C)) {
		continue;
	}
	return UDR0;
	}

void output_ch_0(char data)
{
	while ((STAT0RXTX_REG & (1<<UDR0E)) != (1<<UDR0E)) {
		continue;
	}
	UDR0 = data;

	return;
}



#if (NUM_OF_UARTS == 2)

/*if you want to be able to change the baud rate "on the fly", just add in some
code that calculates the proper baud rate register settings. The calculations
are used above with #define BAUD_RATE1_REG. However, if you want to change
baud rates BUT will only need a few, it would be easier to pre-calculate
the baud register settings in pre-processor, and use a switch statement that
lets you select between a few baud rates*/

void init_uart1 (void)
{
	//turn on TX and RX
   RXTXEN1_REG = (1<<RX1EN) | (1<<TX1EN);

   //set up baud rate
#if (BAUDREGS == 2)
   	BAUD1H_REG = (unsigned char)(BAUD_RATE1_REG >> 8);
   	BAUD1L_REG = (unsigned char)BAUD_RATE1_REG;
#else
   	BAUD1L_REG = (unsigned char)BAUD_RATE1_REG;
#endif
   return;
}

unsigned char input_ch_w_timeout_1(char *data, volatile unsigned int timeout)
{
   unsigned int	timeout_counter = 0;

   //check if a byte has been recieved or if the timeout has been excedded
    while (timeout_counter != timeout) {
		if ((STAT1RXTX_REG & (1<<RX1C)) == (1<<RX1C)) {
			*data = UDR1;
			return BYTE_REC;
		}
		timeout_counter++;
	}

	return TIMEOUT;
}

char input_ch_1(void)
{
   //check if a byte has been recieved or if the timeout has been excedded
    while ((STAT1RXTX_REG & (1<<RX1C)) != (1<<RX1C)) {
	   continue;
    }
	return UDR1;
}

void output_ch_1(char data)
{
	while ((STAT1RXTX_REG & (1<<UDR1E)) != (1<<UDR1E)) {
	   continue;
	}
	UDR1 = data;

	return;
}

#endif
