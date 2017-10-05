/* Simple routine to use the hardware UART. Designed to NOT use interrupts,
this code is extreamly portable, and also extreamly simple.

Created by: Colin O'Flynn
Contact: c_oflynn@yahoo.com or coflynn@newae.com or username c_oflynn on
www.avrfreaks.net

These routines are released free of restrictions, but if anything bad happens
(including but not limited to loss of your time, loss of profit, loss of life,
injury, loss of money, loss of your dog) it is your OWN fault, NO ONE else
can be held responsible*/


//CPU clock speed in Hz
#ifndef F_CPU
#error "Define F_CPU"
#else
#define CPU_CLK_SPEED		F_CPU
#endif

//Baud rate we want for UART0
#define BAUD_RATE0			38400
//Baud rate we want for UART1
#define BAUD_RATE1			115200


/* If the AVR you want to use isn't supported yet, it is very simple to add. If
the AVR has the same register names as another already defined chip (say for
example the AtMega8), then just add another line to the #if statement. So
instead of being
#if 	defined(__AVR_ATmega8__)
//if chip is AtMega8
	#define BAUDREGS			2
	#define BAUD0H_REG		UBRRH
   #define....

it becomes

#if 	defined(__AVR_ATmega8__) || \
		defined(__AVR_ATname__)
//if chip is AtMega8 or ATname
	#define BAUDREGS			2
	#define BAUD0H_REG		UBRRH

If this isn't the case, then look through the data-sheet and map all the register
names to the generals defines used in this program. 
*/

#if 	defined(__AVR_ATmega8__) 	|| \
		defined(__AVR_ATmega16__)	|| \
      defined(__AVR_ATmega32__) 
	#define NUM_OF_BAUDREGS	2
	#define BAUD0H_REG		UBRRH
	#define BAUD0L_REG		UBRRL
   #define NUM_OF_UARTS    1
	#define RXTXEN0_REG		UCSRB
	#define STAT0RXTX_REG 	UCSRA
   #define UDR0				UDR
   #define RX0EN				RXEN
   #define TX0EN				TXEN
   #define RX0C				RXC
   #define UDR0E				UDRE

#elif	defined(__AVR_AT90S4433__)
	#define NUM_OF_BAUDREGS	1
	#define BAUD0L_REG		UBRR
   #define NUM_OF_UARTS    1
	#define RXTXEN0_REG		UCSRB
	#define STAT0RXTX_REG 	UCSRA
   #define UDR0				UDR
   #define RX0EN				RXEN
   #define TX0EN				TXEN
   #define RX0C				RXC
   #define UDR0E				UDRE

#elif defined(__AVR_AT90S8515__) || \
		defined(__AVR_AT90S2313__) || \
      defined(__AVR_AT90S8535__) || \
      defined(__AVR_ATmega103__)
	#define BAUDREGS			1
	#define BAUD0L_REG		UBRR
   #define NUM_OF_UARTS    1
	#define RXTXEN0_REG		UCR
	#define STAT0RXTX_REG 	USR
   #define UDR0				UDR
   #define RX0EN				RXEN
   #define TX0EN				TXEN
   #define RX0C				RXC
   #define UDR0E				UDRE

//if chip is AtMega128
#elif 	defined(__AVR_ATmega128__) || \
		defined(__AVR_ATmega64__) || \
        defined(__AVR_ATmega128RFA1__) || \
        defined(__AVR_AT90CAN128__)
	#define NUM_OF_BAUDREGS	2
	#define BAUD0H_REG		UBRR0H
	#define BAUD0L_REG		UBRR0L
   #define NUM_OF_UARTS    2
	#define RXTXEN0_REG		UCSR0B
	#define STAT0RXTX_REG 	UCSR0A
//  #define UDR0				UDR0 don't need to redefine it
   #define RX0EN				RXEN0
   #define TX0EN				TXEN0
   #define RX0C				RXC0
   #define UDR0E				UDRE0

	#define BAUD1H_REG		UBRR1H
	#define BAUD1L_REG		UBRR1L
	#define RXTXEN1_REG		UCSR1B
	#define STAT1RXTX_REG 	UCSR1A
//  #define UDR1				UDR1 don't need to redefine it
   #define RX1EN				RXEN1
   #define TX1EN				TXEN1
   #define RX1C				RXC1
   #define UDR1E				UDRE1
   
#elif   defined(__AVR_ATmega48__) || \
		defined(__AVR_ATmega88__)  || \
		defined(__AVR_ATmega168__) || \
		defined(__AVR_ATmega328__)  || \
		defined(__AVR_ATmega48P__) || \
		defined(__AVR_ATmega88P__)  || \
		defined(__AVR_ATmega168P__) || \
		defined(__AVR_ATmega328P__)  || \
		defined(__AVR_ATmega48A__) || \
		defined(__AVR_ATmega88A__)  || \
		defined(__AVR_ATmega168A__) || \
		defined(__AVR_ATmega328A__) 
		
	#define NUM_OF_BAUDREGS	2
	#define BAUD0H_REG		UBRR0H
	#define BAUD0L_REG		UBRR0L
   #define NUM_OF_UARTS    1
	#define RXTXEN0_REG		UCSR0B
	#define STAT0RXTX_REG 	UCSR0A
//  #define UDR0				UDR0 don't need to redefine it
   #define RX0EN				RXEN0
   #define TX0EN				TXEN0
   #define RX0C				RXC0
   #define UDR0E				UDRE0
   
#else
	#define NUM_OF_BAUDREGS	2
	#define BAUD0H_REG		UBRR1H
	#define BAUD0L_REG		UBRR1L
   #define NUM_OF_UARTS    1
	#define RXTXEN0_REG		UCSR1B
	#define STAT0RXTX_REG 	UCSR1A
//  #define UDR0				UDR0 don't need to redefine it
   #define RX0EN				RXEN1
   #define TX0EN				TXEN1
   #define RX0C				RXC1
   #define UDR0E				UDRE1

	#define BAUD1H_REG		UBRR1H
	#define BAUD1L_REG		UBRR1L
	#define RXTXEN1_REG		UCSR1B
	#define STAT1RXTX_REG 	UCSR1A
//  #define UDR1				UDR1 don't need to redefine it
   #define RX1EN				RXEN1
   #define TX1EN				TXEN1
   #define RX1C				RXC1
   #define UDR1E				UDRE1

//#else
//	#error "No supported chip type in use for UART.C"
#endif

//Init UART0, set to baud rate as defined in header file
void												init_uart0
	(
   void
   );

//Input a char on UART0 and store it to data, however if no char is recieved
//within timeout, then abort and return TIMEOUT, otherwise return BYTE_REC
//(note: timeout is NOT a reliable value, as it uses a simple C loop that
//will change with different compiler settings likely
unsigned char									input_ch_w_timeout_0
	(
   char *						data,
   unsigned int				timeout
   );

//wait forever for a char on UART 0 and return it
char												input_ch_0
	(
   void
   );

//output char data on UART0
void												output_ch_0
	(
	char							data
	);


#if (NUM_OF_UARTS == 2)
//Init UART1, set to baud rate as defined in header file
void												init_uart1
	(
   void
   );

//Input a char on UART1 and store it to data, however if no char is recieved
//within timeout, then abort and return TIMEOUT, otherwise return BYTE_REC
//(note: timeout is NOT a reliable value, as it uses a simple C loop that
//will change with different compiler settings likely
unsigned char									input_ch_w_timeout_1
	(
   char *						data,
   unsigned int				timeout
   );

//wait forever for a char on UART 1 and return it
char												input_ch_1
	(
   void
   );

//output char data on UART1
void												output_ch_1
	(
	char							data
	);

#endif

//error codes returned by functions
#define BYTE_REC				0
#define TIMEOUT				1