#include "uart.h"

/*! Define that selects the Usart used in example. */
#define USART USARTC0

#define TIMEOUT 0
#define BYTE_REC 1

void												init_uart0
	(
   void
   )
   {
/* This PORT setting is only valid to USARTC0 if other USARTs is used a
	 * different PORT and/or pins is used. */
	/* PIN3 (TXD0) as output. */
	PORTC.DIRSET = PIN3_bm;

	/* PC2 (RXD0) as input. */
	PORTC.DIRCLR = PIN2_bm;

	/* USARTC0, 8 Data bits, No Parity, 1 Stop bit. */
	USART_Format_Set(&USART, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);

	/* Set Baudrate to 9600 bps:
	 * Use the default I/O clock fequency that is 2 MHz.
	 * Do not use the baudrate scale factor
	 *
	 * Baudrate select = (1/(16*(((I/O clock frequency)/Baudrate)-1)
	 *                 = 12
	 */
	#if SS_VER==SS_VER_2_1
	USART_Baudrate_Set(&USART, 1, 0);
	#else
	USART_Baudrate_Set(&USART, 11, 0);
	#endif

	/* Enable both RX and TX. */
	USART_Rx_Enable(&USART);
	USART_Tx_Enable(&USART);
   }

unsigned char									input_ch_w_timeout_0
	(
   char *					 	data,
   volatile unsigned int				timeout
   )
   {
   unsigned int				timeout_counter = 0;

   
   //check if a byte has been recieved or if the timeout has been excedded
   while (timeout_counter != timeout)
		{	
		if (USART_IsRXComplete(&USART))
			{
			*data = USART_GetChar(&USART);
			return BYTE_REC;
			}
		timeout_counter++;
		}
		
	return TIMEOUT;
	}
		
char												input_ch_0
	(
   void
   )
   {
   //check if a byte has been recieved or if the timeout has been excedded
   while (!USART_IsRXComplete(&USART))
		{
		continue;		
		}		
	return USART_GetChar(&USART);;
	}
	
void												output_ch_0
	(
	char							data
	)
	{
	while(!USART_IsTXDataRegisterEmpty(&USART));
	USART_PutChar(&USART, data);
	return;
	}