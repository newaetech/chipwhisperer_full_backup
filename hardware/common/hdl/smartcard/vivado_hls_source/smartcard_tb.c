#include "smartcard.h"
#include "ap_cint.h"

int main()
{
	uint128 command;
	uint128 response;

	uint8 uart_out[32];
	uint8 uart_in[32];
	uint1 status;

	int i;

	uart_in[0] = 0x12;
	uart_in[1] = 0x00;
	uart_in[2] = 0x90;

    command = apint_string2bits_hex("00112233445566778899AABBCCDDEEFF", 128);

	uint16 rv = smartcard(0x80, 0x12, 0x01, 0x02, 0x10, command, 0, &response, &status, uart_out, uart_in);

	for(i=0; i < 16+5; i++) {
		printf("%02x ", uart_out[i]);
	}
	printf("\n");

	printf("status=%d rv=%04x\n", status, rv);

	return 0;
}
