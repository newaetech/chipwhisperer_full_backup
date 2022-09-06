/*
 Copyright (c) 2015-2016 NewAE Technology Inc. All rights reserved.

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
#include "usart_driver.h"
#include "tasks.h"
#include "usb_xmem.h"
#include "usb.h"
#include "tps56520.h"
#include "naeusb_default.h"
#include "naeusb_fpga_target.h"
#include "cdce906.h"
#include <string.h>


//Serial Number - will be read by device ID
char usb_serial_number[33] = "000000000000DEADBEEF";

void fpga_pins(bool enabled);
static void configure_console(void);

// static inline void genclk_enable_config(unsigned int id, enum genclk_source src, unsigned int divider)
// {
//     struct genclk_config gcfg;

//     genclk_config_defaults(&gcfg, id);
//     genclk_enable_source(src);
//     genclk_config_set_source(&gcfg, src);
//     genclk_config_set_divider(&gcfg, divider);
//     genclk_enable(&gcfg, id);
// }

void fpga_pins(bool enabled)
{
	
	gpio_configure_pin(PIO_PB22_IDX, PIO_OUTPUT_0);
	gpio_configure_pin(PIO_PB18_IDX, PIO_OUTPUT_0);
	gpio_configure_pin(PIN_FPGA_DONE_GPIO, PIN_FPGA_DONE_FLAGS);
	
	if (enabled){
		#ifdef CONF_BOARD_PCK0
		gpio_configure_pin(PIN_PCK0, PIN_PCK0_FLAGS);
		#endif

		#ifdef CONF_BOARD_PCK1
		gpio_configure_pin(PIN_PCK1, PIN_PCK1_FLAGS);
		#endif
		
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
			
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A0, PIN_EBI_DATA_BUS_FLAG2);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A1, PIN_EBI_DATA_BUS_FLAG2);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A2, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A3, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A4, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A5, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A6, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A7, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A8, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A9, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A10, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A11, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A12, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A13, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A14, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A15, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A16, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A17, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A18, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A19, PIN_EBI_DATA_BUS_FLAG1);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A20, PIN_EBI_DATA_BUS_FLAG1);
		
		/* FPGA Programming pins */
		FPGA_NPROG_SETUP();
		FPGA_NPROG_HIGH();
			
		/* FPGA External memory interface */
		//Allow sync writing to address pins
		//gpio_configure_group(FPGA_ADDR_PORT, FPGA_ADDR_PINS, (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT));
		//pio_enable_output_write(FPGA_ADDR_PORT, FPGA_ADDR_PINS);
			
		//ALE pin under SW control
		//gpio_configure_pin(FPGA_ALE_GPIO, FPGA_ALE_FLAGS);
		//gpio_set_pin_high(FPGA_ALE_GPIO);
			
		//Force FPGA trigger
		gpio_configure_pin(FPGA_TRIGGER_GPIO, FPGA_TRIGGER_FLAGS);
		
		gpio_configure_pin(PIN_FPGA_PROGRAM_GPIO, PIN_FPGA_PROGRAM_FLAGS);
		
		gpio_configure_pin(SPI_MISO_GPIO, SPI_MISO_FLAGS);
		gpio_configure_pin(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
		gpio_configure_pin(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);
		
	} else {
		#ifdef CONF_BOARD_PCK0
		gpio_configure_pin(PIN_PCK0, PIO_INPUT);
		#endif

		#ifdef CONF_BOARD_PCK1
		gpio_configure_pin(PIN_PCK1, PIO_INPUT);
		#endif
		
		gpio_configure_pin(PIN_EBI_DATA_BUS_D0, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_DATA_BUS_D1, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_DATA_BUS_D2, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_DATA_BUS_D3, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_DATA_BUS_D4, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_DATA_BUS_D5, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_DATA_BUS_D6, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_DATA_BUS_D7, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_NRD, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_NWE, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_NCS0, PIO_INPUT);
		
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A0, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A1, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A2, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A3, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A4, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A5, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A6, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A7, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A8, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A9, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A10, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A11, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A12, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A13, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A14, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A15, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A16, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A17, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A18, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A19, PIO_INPUT);
		gpio_configure_pin(PIN_EBI_ADDR_BUS_A20, PIO_INPUT);	
			
		/* FPGA External memory interface */
		//Allow sync writing to address pins
		gpio_configure_group(FPGA_ADDR_PORT, FPGA_ADDR_PINS, PIO_INPUT);
		
		//ALE pin under SW control
		gpio_configure_pin(FPGA_ALE_GPIO, PIO_INPUT);
		
		//Force FPGA trigger
		gpio_configure_pin(FPGA_TRIGGER_GPIO, PIO_INPUT);
		
		gpio_configure_pin(PIN_FPGA_PROGRAM_GPIO, PIO_INPUT);
		
		gpio_configure_pin(SPI_MISO_GPIO, PIO_INPUT);
		gpio_configure_pin(SPI_MOSI_GPIO, PIO_INPUT);
		gpio_configure_pin(SPI_SPCK_GPIO, PIO_INPUT);
	}
	
}

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	uint32_t serial_number[4];
	
	// Read Device-ID from SAM3U. Do this before enabling interrupts etc.
	flash_read_unique_id(serial_number, sizeof(serial_number));
		
	configure_console();

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

	fpga_program_init();
	tps56520_init();
	
	//Init CDCE906 Chip
	cdce906_init();

	//Convert serial number to ASCII for USB Serial number
	for(unsigned int i = 0; i < 4; i++){
		sprintf(usb_serial_number+(i*8), "%08x", (unsigned int)serial_number[i]);	
	}
	usb_serial_number[32] = 0;
	
	printf("ChipWhisperer-CW305 Online. Firmware build: %s/%s\n", __TIME__, __DATE__);
	printf("Serial number: %s\n", usb_serial_number);
	

	/* Enable SMC */
	pmc_enable_periph_clk(ID_SMC);	
	fpga_pins(true);
	
	/* Configure EBI I/O for PSRAM connection */
	printf("Setting up FPGA Communication\n");
	
	/* complete SMC configuration between PSRAM and SMC waveforms. */
	smc_set_setup_timing(SMC, 0, SMC_SETUP_NWE_SETUP(0)
	| SMC_SETUP_NCS_WR_SETUP(1)
	| SMC_SETUP_NRD_SETUP(1)
	| SMC_SETUP_NCS_RD_SETUP(1));
	smc_set_pulse_timing(SMC, 0, SMC_PULSE_NWE_PULSE(3)
	| SMC_PULSE_NCS_WR_PULSE(1)
	| SMC_PULSE_NRD_PULSE(3)
	| SMC_PULSE_NCS_RD_PULSE(3));
	smc_set_cycle_timing(SMC, 0, SMC_CYCLE_NWE_CYCLE(4)
	| SMC_CYCLE_NRD_CYCLE(4));
	smc_set_mode(SMC, 0, SMC_MODE_READ_MODE | SMC_MODE_WRITE_MODE
        | SMC_MODE_DBW_BIT_8);
	
	
	ui_init();

	// Start USB stack to authorize VBus monitoring
	udc_start();

	/* Enable PCLK0 at 96 MHz */	
	genclk_enable_config(GENCLK_PCK_0, GENCLK_PCK_SRC_MCK, GENCLK_PCK_PRES_1);
	
	//Following is 60MHz version
	//genclk_enable_config(GENCLK_PCK_0, GENCLK_PCK_SRC_PLLBCK, GENCLK_PCK_PRES_4);
	
	printf("Event Loop Entered, waiting...\n");	
	
	//Turn off FPGA pins for now, will be enabled in event loop
	fpga_pins(false);
		
	// The main loop manages only the power mode
	// because the USB management is done by interrupt
	
	naeusb_register_handlers();
    fpga_target_register_handlers();
	while (true) {
		// sleepmgr_enter_sleep();
		process_events();
	}
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

/*
Can insert regular tasks here if needed
*/
void do_task(void)
{
	static bool last_power_state = false;
	
	//If change in external state pin
	if (board_get_powerstate() != last_power_state){
		if (board_get_powerstate()){
			//If power turned on, enable all IO to FPGA
			fpga_pins(true);
		} else {
			//If power turned off, disable all IO to FPGA
			fpga_pins(false);
		}
		
		//Record new state
		last_power_state = board_get_powerstate();
	}
}
