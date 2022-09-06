/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>
#include "compiler.h"
#include "board.h"
#include "conf_board.h"
#include "gpio.h"
#include "ioport.h"
#include "pmc.h"
#include "ioport.h"
#include "genclk.h"

void board_init(void)
{
#ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT 
	/* Disable the watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;
#endif

	/* GPIO has been deprecated, the old code just keeps it for compatibility.
	 * In new designs IOPORT is used instead.
	 * Here IOPORT must be initialized for others to use before setting up IO.
	 */
	ioport_init();

	/* Configure LED pins */
	gpio_configure_pin(LED0_GPIO, LED0_FLAGS);
	gpio_configure_pin(LED1_GPIO, LED1_FLAGS);
	gpio_configure_pin(LED2_GPIO, LED2_FLAGS);
	gpio_configure_pin(LED3_GPIO, LED3_FLAGS);
	
	genclk_enable_source(CONFIG_PLL0_SOURCE);	
	
	gpio_configure_pin(PIO_CAPTURE_ENCTR_IDX, PIO_OUTPUT_1 | PIO_DEFAULT);
	gpio_configure_pin(ADCCLK_EN_IDX, PIO_OUTPUT_0 | PIO_DEFAULT);
	gpio_configure_pin(ADCCLK_SRC_IDX, PIO_OUTPUT_0 | PIO_DEFAULT);
	
	gpio_configure_pin(PIO_CAPTURELATCH_nSET, PIO_OUTPUT_1| PIO_DEFAULT);
	gpio_configure_pin(PIO_CAPTURELATCH_nRESET, PIO_OUTPUT_1| PIO_DEFAULT);

	//Currently not used, so just always let external logic enable capture	
	gpio_set_pin_high(PIO_CAPTURE_ENCTR_IDX);
	

#ifdef CONF_BOARD_UART_CONSOLE
	/* Configure UART pins */
	gpio_configure_group(PINS_UART0_PIO, PINS_UART0, PINS_UART0_FLAGS);
#endif

#ifdef CONF_BOARD_TWI0
	gpio_configure_pin(TWI0_DATA_GPIO, TWI0_DATA_FLAGS);
	gpio_configure_pin(TWI0_CLK_GPIO, TWI0_CLK_FLAGS);
#endif

#ifdef CONF_BOARD_TWI1
	gpio_configure_pin(TWI1_DATA_GPIO, TWI1_DATA_FLAGS);
	gpio_configure_pin(TWI1_CLK_GPIO, TWI1_CLK_FLAGS);
#endif

	/* Configure SPI pins */
#ifdef CONF_BOARD_SPI
	gpio_configure_pin(SPI_MISO_GPIO, SPI_MISO_FLAGS);
	gpio_configure_pin(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
	gpio_configure_pin(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);
	
	/**
	 * For NPCS 1, 2, and 3, different PINs can be used to access the same NPCS line.
	 * Depending on the application requirements, the default PIN may not be available. 
	 * Hence a different PIN should be selected using the CONF_BOARD_SPI_NPCS_GPIO and 
	 * CONF_BOARD_SPI_NPCS_FLAGS macros.
	 */
	
	#ifdef CONF_BOARD_SPI_NPCS0
		gpio_configure_pin(SPI_NPCS0_GPIO, SPI_NPCS0_FLAGS);
	#endif

	#ifdef CONF_BOARD_SPI_NPCS1
		#if defined(CONF_BOARD_SPI_NPCS1_GPIO) && defined(CONF_BOARD_SPI_NPCS1_FLAGS)
			gpio_configure_pin(CONF_BOARD_SPI_NPCS1_GPIO, CONF_BOARD_SPI_NPCS1_FLAGS);
		#else
			gpio_configure_pin(SPI_NPCS1_PA31_GPIO, SPI_NPCS1_PA31_FLAGS);
		#endif
	#endif

	#ifdef CONF_BOARD_SPI_NPCS2
		#if defined(CONF_BOARD_SPI_NPCS2_GPIO) && defined(CONF_BOARD_SPI_NPCS2_FLAGS)
			gpio_configure_pin(CONF_BOARD_SPI_NPCS2_GPIO, CONF_BOARD_SPI_NPCS2_FLAGS);
		#else
			gpio_configure_pin(SPI_NPCS2_PA30_GPIO, SPI_NPCS2_PA30_FLAGS);
		#endif
	#endif

	#ifdef CONF_BOARD_SPI_NPCS3
		#if defined(CONF_BOARD_SPI_NPCS3_GPIO) && defined(CONF_BOARD_SPI_NPCS3_FLAGS)
			gpio_configure_pin(CONF_BOARD_SPI_NPCS3_GPIO, CONF_BOARD_SPI_NPCS3_FLAGS);
		#else
			gpio_configure_pin(SPI_NPCS3_PA22_GPIO, SPI_NPCS3_PA22_FLAGS);
		#endif
	#endif
#endif

#ifdef CONF_BOARD_USART_RXD
	/* Configure USART RXD pin */
	gpio_configure_pin(PIN_USART1_RXD_IDX, PIN_USART1_RXD_FLAGS);
#endif

#ifdef CONF_BOARD_USART_TXD
	/* Configure USART TXD pin */
	gpio_configure_pin(PIN_USART1_TXD_IDX, PIN_USART1_TXD_FLAGS);
#endif

#ifdef CONF_BOARD_USART_CTS
	/* Configure USART CTS pin */
	gpio_configure_pin(PIN_USART1_CTS_IDX, PIN_USART1_CTS_FLAGS);
#endif

#ifdef CONF_BOARD_USART_RTS
	/* Configure USART RTS pin */
	gpio_configure_pin(PIN_USART1_RTS_IDX, PIN_USART1_RTS_FLAGS);
#endif

#ifdef CONF_BOARD_USART_SCK
	/* Configure USART synchronous communication SCK pin */
	gpio_configure_pin(PIN_USART1_SCK_IDX, PIN_USART1_SCK_FLAGS);
#endif


#if defined(CONF_BOARD_ENABLE_MXT143E_XPLAINED)
	pio_configure_pin(MXT143E_XPLAINED_MISO, SPI_MISO_FLAGS);
	pio_configure_pin(MXT143E_XPLAINED_MOSI, SPI_MOSI_FLAGS);
	pio_configure_pin(MXT143E_XPLAINED_SCK, SPI_SPCK_FLAGS);
	pio_configure_pin(MXT143E_XPLAINED_CS, (PIO_OUTPUT_0 | PIO_DEFAULT));
	pio_configure_pin(MXT143E_XPLAINED_CHG, (PIO_INPUT | PIO_PULLUP));
	pio_configure_pin(MXT143E_XPLAINED_DC, (PIO_OUTPUT_0 | PIO_DEFAULT));
	#ifndef MXT143E_XPLAINED_BACKLIGHT_DISABLE
	pio_configure_pin(MXT143E_XPLAINED_BACKLIGHT, (PIO_OUTPUT_0 | PIO_DEFAULT));
	#endif
	pio_configure_pin(MXT143E_XPLAINED_LCD_RESET, (PIO_OUTPUT_0 | PIO_DEFAULT));
	pio_configure_pin(MXT143E_XPLAINED_SDA, TWI0_DATA_FLAGS);
	pio_configure_pin(MXT143E_XPLAINED_SCL, TWI0_CLK_FLAGS);

	pmc_enable_periph_clk(ID_PIOB);
#endif
}
