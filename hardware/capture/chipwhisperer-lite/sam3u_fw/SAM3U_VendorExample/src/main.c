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
#include "pdi\XPROGNewAE.h"
#include "pdi\XPROGTimeout.h"
#include "pdi\XPROGTarget.h"
#include "isp\V2Protocol.h"
#include "usart_driver.h"
#include <string.h>

static volatile bool main_b_vendor_enable = true;

//Serial Number - will be read by device ID
char usb_serial_number[33] = "000000000000DEADBEEF";
/**
 * \name Buffer for loopback
 */
#define  MAIN_LOOPBACK_SIZE    1024

COMPILER_WORD_ALIGNED
		static uint8_t main_buf_loopback[MAIN_LOOPBACK_SIZE];


/* Access pointer for FPGA Interface */
#define PSRAM_BASE_ADDRESS         (0x60000000)
uint8_t volatile *xram = (uint8_t *) PSRAM_BASE_ADDRESS;

void main_vendor_bulk_in_received(udd_ep_status_t status,
		iram_size_t nb_transfered, udd_ep_id_t ep);
void main_vendor_bulk_out_received(udd_ep_status_t status,
		iram_size_t nb_transfered, udd_ep_id_t ep);
		

static void configure_console(void);

void FPGA_setaddr(uint32_t addr)
{
	pio_sync_output_write(FPGA_ADDR_PORT, addr);
	gpio_set_pin_low(FPGA_ALE_GPIO);
	gpio_set_pin_high(FPGA_ALE_GPIO);
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

	/* Enable PCLK0 at 96 MHz */	
	genclk_enable_config(GENCLK_PCK_0, GENCLK_PCK_SRC_MCK, GENCLK_PCK_PRES_1);
	
	//Following is 60MHz version
	//genclk_enable_config(GENCLK_PCK_0, GENCLK_PCK_SRC_PLLBCK, GENCLK_PCK_PRES_4);
	
	printf("Event Loop Entered, waiting...\n");
	
	// The main loop manages only the power mode
	// because the USB management is done by interrupt
	while (true) {
		sleepmgr_enter_sleep();
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

void main_suspend_action(void)
{
	ui_powerdown();
}

void main_resume_action(void)
{
	ui_wakeup();
}

void main_sof_action(void)
{
	if (!main_b_vendor_enable)
		return;
	ui_process(udd_get_frame_number());
}

bool main_vendor_enable(void)
{
	main_b_vendor_enable = true;
	// Start data reception on OUT endpoints
#if UDI_VENDOR_EPS_SIZE_BULK_FS
	//main_vendor_bulk_in_received(UDD_EP_TRANSFER_OK, 0, 0);
	udi_vendor_bulk_out_run(
		main_buf_loopback,
		sizeof(main_buf_loopback),
		main_vendor_bulk_out_received);
#endif
	return true;
}

void main_vendor_disable(void)
{
	main_b_vendor_enable = false;
}

#define REQ_MEMREAD_BULK 0x10
#define REQ_MEMWRITE_BULK 0x11
#define REQ_MEMREAD_CTRL 0x12
#define REQ_MEMWRITE_CTRL 0x13
#define REQ_FPGA_STATUS 0x15
#define REQ_FPGA_PROGRAM 0x16

#define REQ_USART0_DATA 0x1A
#define REQ_USART0_CONFIG 0x1B
#define REQ_USART2_DATA 0x1C
#define REQ_USART2_CONFIG 0x1D

#define REQ_XMEGA_PROGRAM 0x20
#define REQ_AVR_PROGRAM 0x21

COMPILER_WORD_ALIGNED static uint8_t ctrlbuffer[64];
#define CTRLBUFFER_WORDPTR ((uint32_t *) ((void *)ctrlbuffer))

typedef enum {
	bep_emem=0,
	bep_fpgabitstream=10
} blockep_usage_t;

static blockep_usage_t blockendpoint_usage = bep_emem;

static uint8_t * ctrlmemread_buf;
static unsigned int ctrlmemread_size;

void ctrl_readmem_bulk(void);
void ctrl_readmem_ctrl(void);
void ctrl_writemem_bulk(void);
void ctrl_writemem_ctrl(void);
void ctrl_progfpga_bulk(void);
bool ctrl_xmega_program(void);
void ctrl_xmega_program_void(void);
void ctrl_avr_program_void(void);

void ctrl_readmem_bulk(void){
	uint32_t buflen = *(CTRLBUFFER_WORDPTR);	
	uint32_t address = *(CTRLBUFFER_WORDPTR + 1);
	
	/* Set address */
	FPGA_setaddr(address);
	
	/* Do memory read */	
	udi_vendor_bulk_in_run(
	(uint8_t *) PSRAM_BASE_ADDRESS,
	buflen,
	main_vendor_bulk_in_received
	);	
}

void ctrl_readmem_ctrl(void){
	uint32_t buflen = *(CTRLBUFFER_WORDPTR);
	uint32_t address = *(CTRLBUFFER_WORDPTR + 1);
	
	/* Set address */
	FPGA_setaddr(address);

	/* Do memory read */
	ctrlmemread_buf = (uint8_t *) PSRAM_BASE_ADDRESS;
	
	/* Set size to read */
	ctrlmemread_size = buflen;
	
	/* Start Transaction */
}

void ctrl_writemem_ctrl(void){
	uint32_t buflen = *(CTRLBUFFER_WORDPTR);
	uint32_t address = *(CTRLBUFFER_WORDPTR + 1);
	
	uint8_t * ctrlbuf_payload = (uint8_t *)(CTRLBUFFER_WORDPTR + 2);
	
	//printf("Writing to %x, %d\n", address, buflen);
	
	/* Set address */
	FPGA_setaddr(address);

	/* Start Transaction */

	/* Do memory write */
	for(unsigned int i = 0; i < buflen; i++){
		xram[i] = ctrlbuf_payload[i];
	}
}

void ctrl_writemem_bulk(void){
	//uint32_t buflen = *(CTRLBUFFER_WORDPTR);
	uint32_t address = *(CTRLBUFFER_WORDPTR + 1);
	
	/* Set address */
	FPGA_setaddr(address);
	
	/* Transaction done in generic callback */
}

void ctrl_xmega_program_void(void)
{
	XPROGProtocol_Command();
}

void ctrl_avr_program_void(void)
{
	V2Protocol_ProcessCommand();
}


static void ctrl_usart_cb(void)
{
	ctrl_usart(USART0, false);
}

static void ctrl_usart_cb_data(void)
{		
	//Catch heartbleed-style error
	if (udd_g_ctrlreq.req.wLength > udd_g_ctrlreq.payload_size){
		return;
	}
	
	for (int i = 0; i < udd_g_ctrlreq.req.wLength; i++){
		usart_driver_putchar(USART0, NULL, udd_g_ctrlreq.payload[i]);
	}
}


bool main_setup_out_received(void)
{
	//Add buffer if used
	udd_g_ctrlreq.payload = ctrlbuffer;
	udd_g_ctrlreq.payload_size = min(udd_g_ctrlreq.req.wLength,	sizeof(ctrlbuffer));
	
	blockendpoint_usage = bep_emem;
	
	switch(udd_g_ctrlreq.req.bRequest){
		/* Memory Read */
		case REQ_MEMREAD_BULK:
			udd_g_ctrlreq.callback = ctrl_readmem_bulk;
			return true;
		case REQ_MEMREAD_CTRL:
			udd_g_ctrlreq.callback = ctrl_readmem_ctrl;
			return true;	
			
		/* Memory Write */
		case REQ_MEMWRITE_BULK:
			udd_g_ctrlreq.callback = ctrl_writemem_bulk;
			return true;
			
		case REQ_MEMWRITE_CTRL:
			udd_g_ctrlreq.callback = ctrl_writemem_ctrl;
			return true;		
			
		/* Target serial */
		case REQ_USART0_CONFIG:
			udd_g_ctrlreq.callback = ctrl_usart_cb;
			return true;
			
		case REQ_USART0_DATA:
			udd_g_ctrlreq.callback = ctrl_usart_cb_data;
			return true;
		
		/* Smartcard serial */
		case REQ_USART2_CONFIG:
			return true;
			
		/* Send bitstream to FPGA */
		case REQ_FPGA_PROGRAM:
			udd_g_ctrlreq.callback = ctrl_progfpga_bulk;
			return true;
			
		/* XMEGA Programming */
		case REQ_XMEGA_PROGRAM:
			/*
			udd_g_ctrlreq.payload = xmegabuffer;
			udd_g_ctrlreq.payload_size = min(udd_g_ctrlreq.req.wLength,	sizeof(xmegabuffer));
			*/
			udd_g_ctrlreq.callback = ctrl_xmega_program_void;
			return true;

		/* AVR Programming */
		case REQ_AVR_PROGRAM:
			udd_g_ctrlreq.callback = ctrl_avr_program_void;
			return true;
			
		default:
			return false;
	}					
}


void ctrl_progfpga_bulk(void){
	
	switch(udd_g_ctrlreq.req.wValue){
		case 0xA0:
			fpga_program_setup1();			
			break;
			
		case 0xA1:
			/* Waiting on data... */
			fpga_program_setup2();
			blockendpoint_usage = bep_fpgabitstream;
			break;
			
		case 0xA2:
			/* Done */
			blockendpoint_usage = bep_emem;
			break;
			
		default:
			break;
	}
}

/*
udd_g_ctrlreq.req.bRequest == 0


&& (udd_g_ctrlreq.req.bRequest == 0)
&& (0 != udd_g_ctrlreq.req.wLength)
*/

bool main_setup_in_received(void)
{
	/*
	udd_g_ctrlreq.payload = main_buf_loopback;
	udd_g_ctrlreq.payload_size =
			min( udd_g_ctrlreq.req.wLength,
			sizeof(main_buf_loopback) );
	*/
	
	static uint8_t  respbuf[64];
	unsigned int cnt;

	switch(udd_g_ctrlreq.req.bRequest){
		case REQ_MEMREAD_CTRL:
			udd_g_ctrlreq.payload = ctrlmemread_buf;
			udd_g_ctrlreq.payload_size = ctrlmemread_size;
			ctrlmemread_size = 0;
			return true;
			break;
			
		case REQ_FPGA_STATUS:
			respbuf[0] = FPGA_ISDONE();
			respbuf[1] = 0;
			respbuf[2] = 0;
			respbuf[3] = 0;
			udd_g_ctrlreq.payload = respbuf;
			udd_g_ctrlreq.payload_size = 4;
			return true;
			break;
			
		case REQ_XMEGA_PROGRAM:
			return XPROGProtocol_Command();
			break;
			
		case REQ_AVR_PROGRAM:
			return V2Protocol_ProcessCommand();
			break;
			
		case REQ_USART0_CONFIG:
			return ctrl_usart(USART0, true);
			break;
			
		case REQ_USART0_DATA:						
			for(cnt = 0; cnt < udd_g_ctrlreq.req.wLength; cnt++){
				respbuf[cnt] = usart_driver_getchar(USART0);
			}
			udd_g_ctrlreq.payload = respbuf;
			udd_g_ctrlreq.payload_size = cnt;
			return true;
			break;
			
		default:
			return false;
	}
	return false;
}

void main_vendor_bulk_in_received(udd_ep_status_t status,
		iram_size_t nb_transfered, udd_ep_id_t ep)
{
	UNUSED(nb_transfered);
	UNUSED(ep);
	if (UDD_EP_TRANSFER_OK != status) {
		return; // Transfer aborted/error
	}	
}

void main_vendor_bulk_out_received(udd_ep_status_t status,
		iram_size_t nb_transfered, udd_ep_id_t ep)
{
	UNUSED(ep);
	if (UDD_EP_TRANSFER_OK != status) {
		// Transfer aborted
		
		//restart
		udi_vendor_bulk_out_run(
		main_buf_loopback,
		sizeof(main_buf_loopback),
		main_vendor_bulk_out_received);
		
		return;
	}
	
	if (blockendpoint_usage == bep_emem){
		for(unsigned int i = 0; i < nb_transfered; i++){
			xram[i] = main_buf_loopback[i];
		}
	} else if (blockendpoint_usage == bep_fpgabitstream){

		/* Send byte to FPGA - this could eventually be done via SPI */		
		for(unsigned int i = 0; i < nb_transfered; i++){
			fpga_program_sendbyte(main_buf_loopback[i]);
		}
		
		FPGA_CCLK_LOW();
	}
	
	//printf("BULKOUT: %d bytes\n", (int)nb_transfered);
	
	udi_vendor_bulk_out_run(
	main_buf_loopback,
	sizeof(main_buf_loopback),
	main_vendor_bulk_out_received);
}
