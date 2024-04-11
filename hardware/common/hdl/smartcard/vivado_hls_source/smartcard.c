#include "ap_cint.h"
#include "ap_utils.h"


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
		       )
{
#pragma HLS PROTOCOL fixed

#pragma HLS INTERFACE ap_fifo depth=32 port=uart_in
#pragma HLS INTERFACE ap_fifo depth=32 port=uart_out

	uint8 procedure_byte;
	unsigned int loopcnt;
	uint16 resp;
	uint8 p3;
	uint8 indx;

	if (len_command > 16){len_command=16;}
	if (len_response > 16){len_response=16;}

	//If no command, P3 is response size
	if ((len_command == 0) & (len_response > 0)){
		p3 = len_response;
	} else {
		p3 = len_command;
	}


	//Output header, first four bytes
	*(uart_out++) = cla;
	*(uart_out++) = ins;
	*(uart_out++) = p1;
	*(uart_out++) = p2;
	*(uart_out++) = p3;

	ap_wait();

	//Read one-byte input
	/*
	do {
		procedure_byte = *(uart_in + incnt);
	} while (procedure_byte != 0x60);
	*/

	procedure_byte = *(uart_in++);
	if (procedure_byte == ins){
		*status = 1;
	} else {
		*status = 0;
	}

	ap_wait();

	//Output payload if any
	loopcnt = 120;
	for(indx = 0; indx < len_command; indx++){
		*(uart_out++) = apint_get_range(command, loopcnt+7, loopcnt);
		loopcnt -= 8;
	}
	ap_wait();

	if((len_command > 0) & (len_response > 0)) {
		*(uart_out++) = len_response;
		ap_wait();
	}

	//Read payload if any
	loopcnt = 120;
	*response = 0;
	while(len_response--){
		*response = *response << 8;
		*response |= *(uart_in++);
	}

	ap_wait();

	//Read expected response (if any)
	resp = *(uart_in++) << 8;
	resp |= *(uart_in++);

	return resp;
}
