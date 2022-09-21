/*
 * naeusb_nano.c
 *
 * Created: 6/28/2021 9:48:48 PM
 *  Author: adewa
 */ 
#include <asf.h>
#include "naeusb.h"
#include "naeusb_default.h"
#include "naeusb_nano.h"
#include "main.h"
#include "genclk.h"
#include <sysclk.h>
uint32_t max_buffer_size = SIZE_BUFF_RECEPT;

#define GPIO1 (1<<0)
#define GPIO2 (1<<1)
#define GPIO3 (1<<2)
#define GPIO4 (1<<3)
#define GPIOPDIC (1<<4)
#define GPIOPDID (1<<5)
#define GPIOnRST (1<<6)

typedef struct gpio_mapping {
	uint32_t gpio_mask;
	uint32_t gpio_idx;
	uint32_t gpio_input_flag;
} gpio_mapping_t;

static uint8_t * ctrlmemread_buf;
static unsigned int ctrlmemread_size;

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

static void reset_main_clock(void)
{
	struct pll_config pllcfg;
	struct genclk_config gencfg;
	gpio_configure_pin(PIN_TARGET_CLKOUT, (PIO_INPUT | PIO_DEFAULT));
	gpio_configure_pin(PIN_PCK1, (PIO_OUTPUT_0 | PIO_DEFAULT));

	genclk_disable(GENCLK_PCK_1);
	genclk_disable(GENCLK_PCK_2);
	for (volatile uint16_t i = 0; i < 5000; i++);

	genclk_config_defaults(&gencfg, GENCLK_PCK_1);
	genclk_config_set_source(&gencfg, GENCLK_PCK_SRC_PLLBCK);
	genclk_config_set_divider(&gencfg, PMC_PCK_PRES_CLK_1);
	genclk_enable(&gencfg, GENCLK_PCK_1);
	gpio_configure_pin(PIN_PCK1, PIN_PCK1_FLAGS);

	genclk_config_defaults(&gencfg, GENCLK_PCK_2);
	genclk_config_set_source(&gencfg, GENCLK_PCK_SRC_PLLBCK);
	genclk_config_set_divider(&gencfg, PMC_PCK_PRES_CLK_1);
	genclk_enable(&gencfg, GENCLK_PCK_2);
	gpio_configure_pin(PIN_TARGET_CLKOUT, PIN_TARGET_CLKOUT_FLAGS);

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

void main_vendor_bulk_in_received(udd_ep_status_t status,
iram_size_t nb_transfered, udd_ep_id_t ep)
{
	UNUSED(nb_transfered);
	UNUSED(ep);
	

	if (UDD_EP_TRANSFER_OK != status) {
		return; // Transfer aborted/error
	}

}

void nano_readmem_bulk(void){
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


void nano_readmem_ctrl(void){
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

bool nano_setup_out_received(void)
{
	switch (udd_g_ctrlreq.req.bRequest) {
		case REQ_SAM_CFG:
			if ((udd_g_ctrlreq.req.wValue & 0xFF)  == 0xF0) {
				udd_g_ctrlreq.callback = reset_main_clock;
				return true;
			}
			return false;

			case REQ_MEMREAD_BULK:
				udd_g_ctrlreq.callback = nano_readmem_bulk;
				return true;
				break;
				
			case REQ_MEMREAD_CTRL:
				udd_g_ctrlreq.callback = nano_readmem_ctrl;
				return true;
				break;
				
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
	}
	return false;
}

bool nano_setup_in_received(void)
{
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
		
	}
	return false;
}

void nano_register_handlers(void)
{
	naeusb_add_in_handler(nano_setup_in_received);
	naeusb_add_out_handler(nano_setup_out_received);
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