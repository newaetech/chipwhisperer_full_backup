/*
 Copyright (c) 2014-2015 NewAE Technology Inc. All rights reserved.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <asf.h>
#include "conf_usb.h"
#include "stdio_serial.h"
#include "ui.h"
#include "genclk.h"
#include "fpga_program.h"
#include "XPROGNewAE.h"
#include "XPROGTimeout.h"
#include "XPROGTarget.h"
#include "V2Protocol.h"
#include "usart_driver.h"
#include "naeusb_default.h"
#include "naeusb_openadc.h"
#include "naeusb_usart.h"
#include "naeusb_mpsse.h"
#include <string.h>

//Serial Number - will be read by device ID
char usb_serial_number[33] = "000000000000DEADBEEF";
#ifndef RSTC_CR_KEY_PASSWD
#define RSTC_CR_KEY_PASSWD RSTC_CR_KEY(0xA5)
#endif
static void configure_console(void);
volatile uint32_t usb_checked = 0x00;

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	uint32_t serial_number[4];
	
	// Read Device-ID from SAM3U. Do this before enabling interrupts etc.
	flash_read_unique_id(serial_number, sizeof(serial_number));
		
	// configure_console();

	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	sleepmgr_init();
#if !SAMD21 && !SAMR21
	sysclk_init();
	board_init();
#else
	system_init();
#endif

	//Tri-state XPROG pins
	XPROGTarget_DisableTargetPDI();

	//Convert serial number to ASCII for USB Serial number
	for(unsigned int i = 0; i < 4; i++){
		sprintf(usb_serial_number+(i*8), "%08x", (unsigned int)serial_number[i]);	
	}
	usb_serial_number[32] = 0;
	
	printf("ChipWhisperer-Lite Online. Firmware build: %s/%s\n", __TIME__, __DATE__);
	printf("Serial number: %s\n", usb_serial_number);
	

	/* Enable SMC */
	pmc_enable_periph_clk(ID_SMC);	
	gpio_configure_pin(PIN_EBI_DATA_BUS_D0, PIN_EBI_DATA_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D1, PIN_EBI_DATA_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D2, PIN_EBI_DATA_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D3, PIN_EBI_DATA_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D4, PIN_EBI_DATA_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D5, PIN_EBI_DATA_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D6, PIN_EBI_DATA_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D7, PIN_EBI_DATA_BUS_FLAG1);	
	gpio_configure_pin(PIN_EBI_NRD, PIN_EBI_NRD_FLAGS);
	gpio_configure_pin(PIN_EBI_NWE, PIN_EBI_NWE_FLAGS);
	gpio_configure_pin(PIN_EBI_NCS0, PIN_EBI_NCS0_FLAGS);

		
	/* We don't use address mapping so don't enable this */	
	/*
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A1, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A2, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A3, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A4, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A5, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A6, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A7, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A8, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A9, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A10, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A11, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A12, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A13, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A14, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A15, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A16, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A17, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A18, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A19, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A20, PIN_EBI_ADDR_BUS_FLAG2);
	*/	
	
	/* Configure EBI I/O for PSRAM connection */
	printf("Setting up FPGA Communication\n");
	
	/* complete SMC configuration between PSRAM and SMC waveforms. */
	smc_set_setup_timing(SMC, 0, SMC_SETUP_NWE_SETUP(0)
	| SMC_SETUP_NCS_WR_SETUP(1)
	| SMC_SETUP_NRD_SETUP(1)
	| SMC_SETUP_NCS_RD_SETUP(1));
	smc_set_pulse_timing(SMC, 0, SMC_PULSE_NWE_PULSE(1)
	| SMC_PULSE_NCS_WR_PULSE(1)
	| SMC_PULSE_NRD_PULSE(3)
	| SMC_PULSE_NCS_RD_PULSE(1));
	smc_set_cycle_timing(SMC, 0, SMC_CYCLE_NWE_CYCLE(2)
	| SMC_CYCLE_NRD_CYCLE(4));
	smc_set_mode(SMC, 0, SMC_MODE_READ_MODE | SMC_MODE_WRITE_MODE
	| SMC_MODE_DBW_BIT_8);
	
	ui_init();

	// Start USB stack to authorize VBus monitoring
	udc_start();
	board_power(1);

	/* Enable PCLK0 at 96 MHz */	
	genclk_enable_config(GENCLK_PCK_0, GENCLK_PCK_SRC_MCK, GENCLK_PCK_PRES_1);
	
	//Following is 60MHz version
	//genclk_enable_config(GENCLK_PCK_0, GENCLK_PCK_SRC_PLLBCK, GENCLK_PCK_PRES_4);
	
	printf("Event Loop Entered, waiting...\n");
	naeusb_register_handlers();
	naeusart_register_handlers();
	openadc_register_handlers();
	mpsse_register_handlers();
	
	// The main loop manages only the power mode
	// because the USB management is done by interrupt
	while (true) {
        // if we've received stuff on USART, send it back to the PC
		cdc_send_to_pc();
		MPSSE_main_sendrecv_byte();
	}
	return 0;
}

/**
 *  Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	
	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}
