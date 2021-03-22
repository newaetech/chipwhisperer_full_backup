 /*
 * Copyright (c) 2018-2019 NewAE Technology Inc.
 * All rights reserved.
 *
 * If you would like to integrate ChipWhisperer-Nano in your development board,
 * you can receive alternate licensed versions of this project, contact NewAE.
 *
 * ChipWhisperer is a trademark of NewAE Technology Inc., registered in the United
 * States of America, European Union, Peoples Republic of China, and other locations.
 *
 * This project is distributed under the BSD 3-Clause Clear License: 
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or other
 *   materials provided with the distribution.
 *
 * * Neither the name of the author nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
 
#include <asf.h>
#include "conf_usb.h"
//#include "stdio_serial.h"
//#include "ui.h"
#include "genclk.h"
//#include "fpga_program.h"
#include "pdi\XPROGNewAE.h"
#include "pdi\XPROGTimeout.h"
#include "pdi\XPROGTarget.h"
#include "main.h"
#include "usart_driver.h"
//#include "usb.h"
#include <string.h>

#define FW_VER_MAJOR 0
#define FW_VER_MINOR 30
#define FW_VER_DEBUG 0

static volatile bool main_b_vendor_enable = true;
static bool active = false;
static volatile bool cdc_settings_change[2] = {true, true};

#define MAIN_LOOPBACK_SIZE 64

COMPILER_WORD_ALIGNED
		static uint8_t main_buf_loopback[MAIN_LOOPBACK_SIZE];

void main_vendor_bulk_in_received(udd_ep_status_t status,
		iram_size_t nb_transfered, udd_ep_id_t ep);
void main_vendor_bulk_out_received(udd_ep_status_t status,
		iram_size_t nb_transfered, udd_ep_id_t ep);

void main_suspend_action(void)
{
	active = false;
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
	active = true;
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

bool usb_is_enabled(void)
{
	return active;
}

#define REQ_MEMREAD_BULK 0x10
#define REQ_MEMWRITE_BULK 0x11
#define REQ_MEMREAD_CTRL 0x12
#define REQ_MEMWRITE_CTRL 0x13
#define REQ_MEMSTREAM 0x14
#define REQ_FPGA_STATUS 0x15
#define REQ_FPGA_PROGRAM 0x16
#define REQ_FW_VERSION 0x17
#define REQ_USART0_DATA 0x1A
#define REQ_USART0_CONFIG 0x1B
#define REQ_SCARD_DATA 0x1C
#define REQ_SCARD_CONFIG 0x1D
#define REQ_SCARD_AUX 0x1E
#define REQ_USART2DUMP_ENABLE 0x1F
#define REQ_XMEGA_PROGRAM 0x20
#define REQ_AVR_PROGRAM 0x21
#define REQ_SAM3U_CFG 0x22
#define REQ_CC_PROGRAM 0x23
#define REQ_GPIO_DIR 0x24
#define REQ_GPIO_OUT 0x25
#define REQ_GPIO_IN  0x26
#define REQ_CLK_OUT  0x27
#define REQ_ADCCLK_OUT 0x28
#define REQ_ARM 0x29
#define REQ_SAMPLES 0x2A
#define REQ_BUFSIZE 0x2B
#define REQ_GLITCHSET 0x2C
#define REQ_GLITCHGO 0x2D
#define REQ_CDC_SETTINGS_EN 0x31

uint32_t max_buffer_size = SIZE_BUFF_RECEPT;

#define GPIO1 (1<<0)
#define GPIO2 (1<<1)
#define GPIO3 (1<<2)
#define GPIO4 (1<<3)
#define GPIOPDIC (1<<4)
#define GPIOPDID (1<<5)
#define GPIOnRST (1<<6)

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
void ctrl_streammem_ctrl(void);
void ctrl_progfpga_bulk(void);
bool ctrl_xmega_program(void);
void ctrl_xmega_program_void(void);
void ctrl_avr_program_void(void);
void ctrl_cc_program_void(void);


void ctrl_xmega_program_void(void)
{
	XPROGProtocol_Command();
}

void ctrl_avr_program_void(void)
{
	;//V2Protocol_ProcessCommand();
}


typedef struct gpio_mapping {
	uint32_t gpio_mask;
	uint32_t gpio_idx;
	uint32_t gpio_input_flag;
} gpio_mapping_t;

#define NUM_GPIO_PINS 8

gpio_mapping_t gpio_mask_list[8] = {
	//{.gpio_mask = GPIO1,    .gpio_idx = PIN_TARGET_GPIO1_IDX, .gpio_input_flag = (PIO_INPUT | PIO_DEFAULT)},
	//{.gpio_mask = GPIO2,    .gpio_idx = PIN_TARGET_GPIO2_IDX, .gpio_input_flag = (PIO_INPUT | PIO_DEFAULT)},
	{.gpio_mask = GPIO3,    .gpio_idx = PIN_TARGET_GPIO3_IDX, .gpio_input_flag = (PIO_INPUT | PIO_DEFAULT)},
	//{.gpio_mask = GPIO4,    .gpio_idx = PIN_TARGET_GPIO4_IDX, .gpio_input_flag = (PIO_INPUT | PIO_DEFAULT)},
	{.gpio_mask = GPIOnRST, .gpio_idx = PIN_TARGET_nRST_IDX, .gpio_input_flag = (PIO_INPUT | PIO_PULLUP | PIO_DEFAULT)},
	{.gpio_mask = GPIOPDIC, .gpio_idx = PIN_TARGET_PDIC_IDX, .gpio_input_flag = (PIO_INPUT | PIO_DEFAULT)},
	{.gpio_mask = GPIOPDID, .gpio_idx = PIN_TARGET_PDID_IDX, .gpio_input_flag = (PIO_INPUT | PIO_DEFAULT)},

};

static void ctrl_sam3ucfg_cb(void)
{
	switch(udd_g_ctrlreq.req.wValue & 0xFF)
	{
		
		/* Jump to ROM-resident bootloader */
		case 0x03:
			/* Clear ROM-mapping bit. */
			efc_perform_command(EFC0, EFC_FCMD_CGPB, 1);
		
			/* Disconnect USB (will kill connection) */
			udc_detach();
		
			/* With knowledge that I will rise again, I lay down my life. */
			while (RSTC->RSTC_SR & RSTC_SR_SRCMP);
			RSTC->RSTC_CR |= RSTC_CR_KEY_PASSWD | RSTC_CR_PERRST | RSTC_CR_PROCRST;
			while(1);
			break;
		
		/* Oh well, sucks to be you */
		default:
			break;
	}
}

static void ctrl_cdc_settings_cb(void)
{
    if (udd_g_ctrlreq.req.wValue & 0x01) {
        cdc_settings_change[0] = 1;
    } else {
        cdc_settings_change[0] = 0;
    }
    if (udd_g_ctrlreq.req.wValue & 0x02) {
        cdc_settings_change[1] = 1;
    } else {
        cdc_settings_change[1] = 0;
    }
}

static void ctrl_usart_cb(void)
{
	ctrl_usart(USART_TARGET, false);
}

static void ctrl_usart_cb_data(void)
{		
	//Catch heartbleed-style error
	if (udd_g_ctrlreq.req.wLength > udd_g_ctrlreq.payload_size){
		return;
	}
	
	for (int i = 0; i < udd_g_ctrlreq.req.wLength; i++){
		usart_driver_putchar(USART_TARGET, NULL, udd_g_ctrlreq.payload[i]);
	}
}

#define PINMODE_OUTPUT 0x01
#define PINMODE_INPUT  0x02
#define PINMODE_LOW    0x03
#define PINMODE_HIGH   0x04
#define PINMODE_PERA   0x05
#define PINMODE_PERB   0x06

void init_gpio_as_input(void)
{
	for (unsigned int i = 0; i < NUM_GPIO_PINS; i++){
		if (gpio_mask_list[i].gpio_mask) {
			gpio_configure_pin(gpio_mask_list[i].gpio_idx, gpio_mask_list[i].gpio_input_flag);
		}
	}
	
}

static void ctrl_gpio_out_cb(void)
{
	//Catch heartbleed-style error
	if (udd_g_ctrlreq.req.wLength > udd_g_ctrlreq.payload_size){
		return;
	}

	if (udd_g_ctrlreq.payload_size > 0){
		
		for (int i = 0; i < 8; i++){
			
			if (udd_g_ctrlreq.payload[0] & gpio_mask_list[i].gpio_mask) {
				
				switch(udd_g_ctrlreq.req.wValue & 0xFF) {
					case PINMODE_OUTPUT:
						gpio_configure_pin(gpio_mask_list[i].gpio_idx, PIO_OUTPUT_0 | PIO_DEFAULT);
						break;
					case PINMODE_INPUT:
						gpio_configure_pin(gpio_mask_list[i].gpio_idx, gpio_mask_list[i].gpio_input_flag);
						break;
					case PINMODE_LOW:
						gpio_set_pin_low(gpio_mask_list[i].gpio_idx);
						break;
					case PINMODE_HIGH:
						gpio_set_pin_high(gpio_mask_list[i].gpio_idx);
						break;						
					case PINMODE_PERA:
						gpio_configure_pin(gpio_mask_list[i].gpio_idx, PIO_PERIPH_A | PIO_DEFAULT);
						break;						
					case PINMODE_PERB:
						gpio_configure_pin(gpio_mask_list[i].gpio_idx, PIO_PERIPH_B | PIO_DEFAULT);
						break;
				}
				
			}
		}
	}
	
}

static unsigned int genclk_tout_div = 0;

static void ctrl_clk_out_cb(void)
{
	//Catch heartbleed-style error
	if (udd_g_ctrlreq.req.wLength > udd_g_ctrlreq.payload_size){
		return;
	}
	
	/* */
	unsigned int pclk_out_div = udd_g_ctrlreq.payload[0];
	//unsigned int pllb_mul = udd_g_ctrlreq.payload[1];
	//unsigned int pllb_div = udd_g_ctrlreq.payload[2];

	struct genclk_config gencfg;
	struct pll_config pcfg;
		

	genclk_tout_div = pclk_out_div;
	

	/* PCLK2 OUT */
	if(pclk_out_div){
		
		/* Convert to expected format */
		switch(pclk_out_div){
			case 1: pclk_out_div = PMC_PCK_PRES_CLK_1; break; 
			case 2: pclk_out_div = PMC_PCK_PRES_CLK_2; break; 
			case 4: pclk_out_div = PMC_PCK_PRES_CLK_4; break; 
			case 8: pclk_out_div = PMC_PCK_PRES_CLK_8; break; 
			case 16: pclk_out_div = PMC_PCK_PRES_CLK_16; break; 
			case 32: pclk_out_div = PMC_PCK_PRES_CLK_32; break; 
			case 64: pclk_out_div = PMC_PCK_PRES_CLK_64; break; 
			default: pclk_out_div = PMC_PCK_PRES_CLK_64;
		}
		
		genclk_config_defaults(&gencfg, GENCLK_PCK_2);
		genclk_config_set_source(&gencfg, GENCLK_PCK_SRC_PLLACK);
		genclk_config_set_divider(&gencfg, pclk_out_div);
		genclk_enable(&gencfg, GENCLK_PCK_2);
		gpio_configure_pin(PIN_TARGET_CLKOUT, PIN_TARGET_CLKOUT_FLAGS);
	} else {
		gpio_configure_pin(PIN_TARGET_CLKOUT, (PIO_INPUT | PIO_DEFAULT));
	}
}

static void ctrl_arm_cb(void)
{
	if(udd_g_ctrlreq.req.wValue == 1){
		adc_capture_start();
	}
}

static unsigned int adcclk_tout_div = 0;
static unsigned int adcclk_src = ADC_CLKSRC_INT;
static unsigned int adcclk_en = 0;

static void ctrl_adcclk_out_cb(void)
{
	//Catch heartbleed-style error
	if (udd_g_ctrlreq.req.wLength > udd_g_ctrlreq.payload_size){
		return;
	}
	
	/* */
	unsigned int pclk_out_div = udd_g_ctrlreq.payload[0];
	//unsigned int pllb_mul = udd_g_ctrlreq.payload[1];
	//unsigned int pllb_div = udd_g_ctrlreq.payload[2];	

	struct genclk_config gencfg;
	struct pll_config pcfg;
	

	adcclk_src = udd_g_ctrlreq.payload[3];	
	adcclk_en  = udd_g_ctrlreq.payload[4];
	adcclk_tout_div = pclk_out_div;
	

	/* PCLK1 OUT */
	if(pclk_out_div){
		
		/* Convert to expected format */
		switch(pclk_out_div){
			case 1: pclk_out_div = PMC_PCK_PRES_CLK_1; break;
			case 2: pclk_out_div = PMC_PCK_PRES_CLK_2; break;
			case 4: pclk_out_div = PMC_PCK_PRES_CLK_4; break;
			case 8: pclk_out_div = PMC_PCK_PRES_CLK_8; break;
			case 16: pclk_out_div = PMC_PCK_PRES_CLK_16; break;
			case 32: pclk_out_div = PMC_PCK_PRES_CLK_32; break;
			case 64: pclk_out_div = PMC_PCK_PRES_CLK_64; break;
			default: pclk_out_div = PMC_PCK_PRES_CLK_64;
		}
		
		genclk_config_defaults(&gencfg, GENCLK_PCK_1);
		genclk_config_set_source(&gencfg, GENCLK_PCK_SRC_PLLACK);
		genclk_config_set_divider(&gencfg, pclk_out_div);
		genclk_enable(&gencfg, GENCLK_PCK_1);
		gpio_configure_pin(PIN_PCK1, PIN_PCK1_FLAGS);
	} else {
		gpio_configure_pin(PIN_PCK1, (PIO_OUTPUT_0 | PIO_DEFAULT));
	}
	
	adc_set_clken(adcclk_en);
	adc_set_clksrc(adcclk_src);
}

static void ctrl_samples_cb(void)
{
	//Catch heartbleed-style error
	if (udd_g_ctrlreq.req.wLength > udd_g_ctrlreq.payload_size){
		return;
	}
	
	if (udd_g_ctrlreq.req.wLength != 4){
		return;
	}
	
	capture_req_length  = *(CTRLBUFFER_WORDPTR);
}

uint32_t glitch_offset;
uint32_t glitch_width;

static void ctrl_glitch_settings(void)
{
	//Catch heartbleed-style error
	if (udd_g_ctrlreq.req.wLength > udd_g_ctrlreq.payload_size){
		return;
	}
	
	glitch_offset = *(CTRLBUFFER_WORDPTR);	
	glitch_width = *(CTRLBUFFER_WORDPTR + 1);

	
	cwnano_glitch_init();
	cwnano_setup_glitch(glitch_offset, glitch_width);
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
			break;
				
		case REQ_MEMREAD_CTRL:
			udd_g_ctrlreq.callback = ctrl_readmem_ctrl;
			return true;					
			break;
			
		/* Memory Write */
		case REQ_MEMWRITE_BULK:
			//udd_g_ctrlreq.callback = ctrl_writemem_bulk;
			return true;
			break;
					
		case REQ_MEMWRITE_CTRL:
			//udd_g_ctrlreq.callback = ctrl_writemem_ctrl;
			return true;					
			break;
			
		/* Target serial */
		case REQ_USART0_CONFIG:
			udd_g_ctrlreq.callback = ctrl_usart_cb;
			return true;
			
		case REQ_USART0_DATA:
			udd_g_ctrlreq.callback = ctrl_usart_cb_data;
			return true;
			
		case REQ_GPIO_OUT:
			udd_g_ctrlreq.callback = ctrl_gpio_out_cb;
			return true;
			
		case REQ_CLK_OUT:
			udd_g_ctrlreq.callback = ctrl_clk_out_cb;
			return true;
	
		case REQ_ADCCLK_OUT:
			udd_g_ctrlreq.callback = ctrl_adcclk_out_cb;
			return true;	
			
		case REQ_ARM:
			udd_g_ctrlreq.callback = ctrl_arm_cb;
			return true;
			
		case REQ_SAMPLES:
			udd_g_ctrlreq.callback = ctrl_samples_cb;
			return true;
			
		case REQ_GLITCHSET:
			udd_g_ctrlreq.callback = ctrl_glitch_settings;
			return true;
			
		case REQ_GLITCHGO:
			udd_g_ctrlreq.callback = cwnano_glitch_insert;
			return true;
			
		/* Send bitstream to FPGA */
		//case REQ_FPGA_PROGRAM:
		//	udd_g_ctrlreq.callback = ctrl_progfpga_bulk;
		//	return true;
			
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
			
		/* Misc hardware setup */
		case REQ_SAM3U_CFG:
			udd_g_ctrlreq.callback = ctrl_sam3ucfg_cb;
			return true;
			
		case REQ_CDC_SETTINGS_EN:
			udd_g_ctrlreq.callback = ctrl_cdc_settings_cb;
			return true;
		default:
			return false;
	}			
	
	// If any of the above failed...
	return false;		
}

bool main_setup_in_received(void)
{
	
	static uint8_t  respbuf[CIRCBUFSIZE];
	unsigned int cnt;
	
	if (udd_g_ctrlreq.req.wLength > sizeof(respbuf)){
		return false;
	}

	switch(udd_g_ctrlreq.req.bRequest){
		case REQ_MEMREAD_CTRL:
			udd_g_ctrlreq.payload = ctrlmemread_buf;
			udd_g_ctrlreq.payload_size = ctrlmemread_size;
			ctrlmemread_size = 0;

			return true;
			break;
			
		case REQ_XMEGA_PROGRAM:
			return XPROGProtocol_Command();
			break;
			
		//case REQ_AVR_PROGRAM:
		//	return V2Protocol_ProcessCommand();
		//	break;
			
		case REQ_USART0_CONFIG:
			return ctrl_usart(USART_TARGET, true);
			break;
			
		case REQ_USART0_DATA:						
			for(cnt = 0; cnt < udd_g_ctrlreq.req.wLength; cnt++){
				respbuf[cnt] = usart_driver_getchar(USART_TARGET);
			}
			udd_g_ctrlreq.payload = respbuf;
			udd_g_ctrlreq.payload_size = cnt;
			return true;
			break;

		case REQ_FW_VERSION:
			respbuf[0] = FW_VER_MAJOR;
			respbuf[1] = FW_VER_MINOR;
			respbuf[2] = FW_VER_DEBUG;
			udd_g_ctrlreq.payload = respbuf;
			udd_g_ctrlreq.payload_size = 3;
			return true;
			break;
			
		case REQ_CLK_OUT:
			respbuf[0] = genclk_tout_div;
			respbuf[1] = 0;
			respbuf[2] = 0;
			udd_g_ctrlreq.payload = respbuf;
			udd_g_ctrlreq.payload_size = 3;
			return true;
			break;

		case REQ_ADCCLK_OUT:
			respbuf[0] = adcclk_tout_div;
			respbuf[1] = 0;
			respbuf[2] = 0;
			respbuf[3] = adcclk_src;
			respbuf[4] = adcclk_en;
			udd_g_ctrlreq.payload = respbuf;
			udd_g_ctrlreq.payload_size = 5;
			return true;
			break;
			
		case REQ_ARM:
			respbuf[0] = adc_capture_done();
			udd_g_ctrlreq.payload = respbuf;
			udd_g_ctrlreq.payload_size = 1;
			return true;
			break;
			
		case REQ_SAMPLES:
			udd_g_ctrlreq.payload = (uint8_t *)&capture_req_length;
			udd_g_ctrlreq.payload_size = sizeof(capture_req_length);
			return true;
			break;
			
		case REQ_BUFSIZE:
			udd_g_ctrlreq.payload = (uint8_t *)&max_buffer_size;
			udd_g_ctrlreq.payload_size = sizeof(max_buffer_size);			
			return true;
			break;
			
		case REQ_GPIO_DIR:
			//Read mode/set state (same as used with REQ_GPIO_OUT request)
			return true;
			break;
			
		case REQ_GPIO_IN:
			//Read pin state
			return true;
			break;
			
		case REQ_GLITCHSET:
			*((uint32_t *)respbuf) = glitch_offset;
			*(((uint32_t *)respbuf)+1) = glitch_width;
			udd_g_ctrlreq.payload = respbuf;
			udd_g_ctrlreq.payload_size = 8;
			return true;
			break;		

        case REQ_CDC_SETTINGS_EN:
            respbuf[0] = cdc_settings_change[0];
            respbuf[1] = cdc_settings_change[1];
            udd_g_ctrlreq.payload = respbuf;
            udd_g_ctrlreq.payload_size = 2;
            return true;
            break;
			
		default:
			return false;
	}
	return false;
}


void ctrl_readmem_bulk(void){
	uint32_t buflen = *(CTRLBUFFER_WORDPTR);	
	//uint32_t address = *(CTRLBUFFER_WORDPTR + 1);
	
	if(buflen > SIZE_BUFF_RECEPT){
		buflen = SIZE_BUFF_RECEPT;
	}
		
	/* Do memory read */	
	udi_vendor_bulk_in_run(
		pio_rx_buffer,
		buflen,
		main_vendor_bulk_in_received
	);	
}


void ctrl_readmem_ctrl(void){
	uint32_t buflen = *(CTRLBUFFER_WORDPTR);
	//uint32_t address = *(CTRLBUFFER_WORDPTR + 1);

	if(buflen > SIZE_BUFF_RECEPT){
		buflen = SIZE_BUFF_RECEPT;
	}
	
	/* Do memory read */
	ctrlmemread_buf = pio_rx_buffer;
	
	/* Set size to read */
	ctrlmemread_size = buflen;
	
	/* Start Transaction */
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
	
	
	//printf("BULKOUT: %d bytes\n", (int)nb_transfered);
	
	udi_vendor_bulk_out_run(
	main_buf_loopback,
	sizeof(main_buf_loopback),
	main_vendor_bulk_out_received);
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// USB CDC
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
#include "usb_protocol_cdc.h"
volatile bool enable_cdc_transfer[2] = {false, false};
	extern volatile bool usart_x_enabled[4];
bool cdc_enable(uint8_t port)
{
	enable_cdc_transfer[port] = true;
	return true;
}

void cdc_disable(uint8_t port)
{
	enable_cdc_transfer[port] = false;
}

/*
		case REQ_USART0_DATA:
		for(cnt = 0; cnt < udd_g_ctrlreq.req.wLength; cnt++){
			respbuf[cnt] = usart_driver_getchar(USART_TARGET);
		}
		udd_g_ctrlreq.payload = respbuf;
		udd_g_ctrlreq.payload_size = cnt;
		return true;
		break;
		
			//Catch heartbleed-style error
			if (udd_g_ctrlreq.req.wLength > udd_g_ctrlreq.payload_size){
				return;
			}
			
			for (int i = 0; i < udd_g_ctrlreq.req.wLength; i++){
				usart_driver_putchar(USART_TARGET, NULL, udd_g_ctrlreq.payload[i]);
			}
*/
static uint8_t uart_buf[64] = {0};
void my_callback_rx_notify(uint8_t port)
{
	//iram_size_t udi_cdc_multi_get_nb_received_data
    if (port > 0)
        return;
	
	if (enable_cdc_transfer[port] && usart_x_enabled[0]) {
		iram_size_t num_char = udi_cdc_multi_get_nb_received_data(port);
		while (num_char > 0) {
			num_char = (num_char > 64) ? 64 : num_char;
			udi_cdc_multi_read_buf(port, uart_buf, num_char);
			for (uint16_t i = 0; i < num_char; i++) { //num_char; num_char > 0; num_char--) {
				//usart_driver_putchar(USART_TARGET, NULL, udi_cdc_multi_getc(port));
				usart_driver_putchar(USART_TARGET, NULL, uart_buf[i]);
			}
			num_char = udi_cdc_multi_get_nb_received_data(port);
		}
	}
}

extern tcirc_buf rx0buf, tx0buf;
extern tcirc_buf usb_usart_circ_buf;

void my_callback_config(uint8_t port, usb_cdc_line_coding_t * cfg)
{
    if (port > 0)
        return;
	if (enable_cdc_transfer[port] && cdc_settings_change[port]) {
        usart_x_enabled[0] = true;
		sam_usart_opt_t usartopts;
		if (port != 0){
			return;
		}
		if (cfg->bDataBits < 5)
			return;
		if (cfg->bCharFormat > 2)
			return;

		usartopts.baudrate = cfg->dwDTERate;
		usartopts.channel_mode = US_MR_CHMODE_NORMAL;
		usartopts.stop_bits = ((uint32_t)cfg->bCharFormat) << 12;
		usartopts.char_length = ((uint32_t)cfg->bDataBits - 5) << 6;
		switch(cfg->bParityType) {
			case CDC_PAR_NONE:
			usartopts.parity_type = US_MR_PAR_NO;
			break;
			case CDC_PAR_ODD:
			usartopts.parity_type = US_MR_PAR_ODD;
			break;
			case CDC_PAR_EVEN:
			usartopts.parity_type = US_MR_PAR_EVEN;
			break;
			case CDC_PAR_MARK:
			usartopts.parity_type = US_MR_PAR_MARK;
			break;
			case CDC_PAR_SPACE:
			usartopts.parity_type = US_MR_PAR_SPACE;
			break;
			default:
			return;
		}
		if (port == 0)
		{
			//completely restart USART - otherwise breaks tx or stalls
			sysclk_enable_peripheral_clock(ID_USART1);
			init_circ_buf(&usb_usart_circ_buf);
			init_circ_buf(&tx0buf);
			init_circ_buf(&rx0buf);
			usart_init_rs232(USART1, &usartopts,  sysclk_get_cpu_hz());
			
			usart_enable_rx(USART1);
			usart_enable_tx(USART1);
			
			usart_enable_interrupt(USART1, UART_IER_RXRDY);
			
			gpio_configure_pin(PIN_USART1_RXD_IDX, PIN_USART1_RXD_FLAGS);
			gpio_configure_pin(PIN_USART1_TXD_IDX, PIN_USART1_TXD_FLAGS);
			irq_register_handler(USART1_IRQn, 5);
		}
	}
		
}