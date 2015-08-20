/*
 * scard.c
 *
 * Created: 01/04/2015 2:11:53 PM
 *  Author: colin
 */ 

#include <asf.h>
#include <iso7816.h>

	/* Hack for smartcard testing */
#define ISO7816_BAUDRATE 9600
#define ISO7816_FI_DI 372
	
void test_smartcard(void)
{
	usart_iso7816_opt_t conf_iso7816_t;	
	conf_iso7816_t.iso7816_hz = ISO7816_BAUDRATE *ISO7816_FI_DI;
	conf_iso7816_t.fidi_ratio = ISO7816_FI_DI;
	conf_iso7816_t.parity_type= US_MR_PAR_EVEN;
	conf_iso7816_t.inhibit_nack = 0x00;
	conf_iso7816_t.dis_suc_nack = 0x00;
	conf_iso7816_t.max_iterations = 0x03;
	conf_iso7816_t.bit_order = 0x00;
	conf_iso7816_t.protocol_type = 0x00;	

	//ISO7816_USART
	sysclk_enable_peripheral_clock(ID_USART2);
	gpio_configure_pin(PIN_USART2_TXD, PIN_USART2_TXD_FLAGS);
	gpio_configure_pin(PIN_USART2_SCK, PIN_USART2_SCK_FLAGS);
	gpio_configure_pin(PIN_ISO7816_RST_IDX, PIN_ISO7816_RST_FLAG);	
	if(iso7816_init(&conf_iso7816_t, sysclk_get_cpu_hz(), PIN_ISO7816_RST_IDX)){
		printf("SCARD: Init Fail\n");
	} else {
		printf("SCARD: Init OK\n");
	}
	
	
	//Reset card
	uint8_t atr[64];
	uint8_t ucsize;
	iso7816_cold_reset();
	iso7816_data_block_atr(atr, &ucsize);
	
	printf("ATR (%d len): ", ucsize);
	for(unsigned int i = 0; i < ucsize; i++){
		printf("%02x ", atr[i]);
	}
	printf("\n");	
}
	