
#include "usart_driver.h"
#include "avr_compiler.h"

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