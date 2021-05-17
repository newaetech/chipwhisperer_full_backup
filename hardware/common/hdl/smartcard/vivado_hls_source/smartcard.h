#include "ap_cint.h"


uint16 smartcard(uint8 cla, 				/* APDU Header: CLA Byte */
		       uint8 ins,				/* APDU Header: INS Byte */
		       uint8 p1,				/* APDU Header: P1 Byte */
		       uint8 p2,				/* APDU Header: P2 Byte */
		       uint5 len_command,		/* APDU Header: Length of Command */
		       uint128 command,			/* Command Data (max 16 bytes) */
		       uint5 len_response,		/* Expected Length of Response */
		       uint128 * response,		/* Response Data */

		       uint1 *   status,

		       volatile uint8 * uart_out,		/* To UART, streaming */
		       volatile uint8 * uart_in			/* From UART, streaming */
		       );