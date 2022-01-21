/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

// External oscillator settings.
// Uncomment and set correct values if external oscillator is used.

// External oscillator frequency
#define BOARD_XOSC_HZ          12000000

// External oscillator type.
//!< External clock signal
//#define BOARD_XOSC_TYPE        XOSC_TYPE_EXTERNAL
//!< 32.768 kHz resonator on TOSC
//#define BOARD_XOSC_TYPE        XOSC_TYPE_32KHZ
//!< 0.4 to 16 MHz resonator on XTALS
#define BOARD_XOSC_TYPE        XOSC_TYPE_XTAL

// External oscillator startup time
#define BOARD_XOSC_STARTUP_US  500000
#define BOARD_FREQ_SLCK_XTAL      (32768UL)
#define BOARD_FREQ_SLCK_BYPASS    (32768UL)
#define BOARD_FREQ_MAINCK_XTAL    (12000000UL)
#define BOARD_FREQ_MAINCK_BYPASS  (12000000UL)
#define BOARD_OSC_STARTUP_US      (15625UL)



/*----------------------------------------------------------------------------*/
/**
 * \page sam4s_xplained_board_info "SAM4S-XPLAINED - Board informations"
 * This page lists several definition related to the board description.
 *
 * \section Definitions
 * - \ref BOARD_NAME
 */

/** Name of the board */
#define BOARD_NAME "CHIPWHISPERER-NANO"
/** Board definition */
#define cwnano
/** Family definition (already defined) */
#define sam4s
/** Core definition */
#define cortexm3

#define CW_USE_USART1

/*----------------------------------------------------------------------------*/


/** UART0 pins (UTXD0 and URXD0) definitions, PA9,10. */
#define PINS_UART0         (PIO_PA9A_URXD0 | PIO_PA10A_UTXD0)
#define PINS_UART0_FLAGS   (PIO_PERIPH_A | PIO_DEFAULT)

#define PINS_UART0_MASK    PIO_PA9A_URXD0|PIO_PA10A_UTXD0
#define PINS_UART0_PIO     PIOA
#define PINS_UART0_ID      ID_PIOA
#define PINS_UART0_TYPE    PIO_PERIPH_A
#define PINS_UART0_ATTR    PIO_DEFAULT

/** UART1 pins (UTXD1 and URXD1) definitions, PB2,PB3. */
#define PINS_UART1         (PIO_PB2A_URXD1 | PIO_PB3A_UTXD1)
#define PINS_UART1_FLAGS   (PIO_PERIPH_A | PIO_DEFAULT)

#define PINS_UART1_MASK    PIO_PB2A_URXD1 | PIO_PB3A_UTXD1
#define PINS_UART1_PIO     PIOB
#define PINS_UART1_ID      ID_PIOB
#define PINS_UART1_TYPE    PIO_PERIPH_A
#define PINS_UART1_ATTR    PIO_DEFAULT

/** LED #0 pin definition. */
#define LED_0_NAME     "LED USB"
#define PIN_LED_0      {PIO_PB0, PIOB, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_LED_0_MASK PIO_PB0
#define PIN_LED_0_PIO  PIOB
#define PIN_LED_0_ID   ID_PIOB
#define PIN_LED_0_TYPE PIO_OUTPUT_0
#define PIN_LED_0_ATTR PIO_DEFAULT

#define LED0_GPIO  (PIO_PB0_IDX)
#define LED0_FLAGS (PIO_OUTPUT_0 | PIO_DEFAULT)
#define LED0_ACTIVE_LEVEL 1


/** LED #1 pin definition. */
#define LED_1_NAME     "LED ERROR"
#define PIN_LED_1      {PIO_PB1, PIOB, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_LED_1_MASK PIO_PB1
#define PIN_LED_1_PIO  PIOB
#define PIN_LED_1_ID   ID_PIOB
#define PIN_LED_1_TYPE PIO_OUTPUT_0
#define PIN_LED_1_ATTR PIO_DEFAULT

#define LED1_GPIO  (PIO_PB1_IDX)
#define LED1_FLAGS (PIO_OUTPUT_0 | PIO_DEFAULT)
#define LED1_ACTIVE_LEVEL 1

#define LED

/** LED #2 pin definition. */
#define LED_2_NAME     "LED ARM"
#define PIN_LED_2      {PIO_PB2, PIOB, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_LED_2_MASK PIO_PB2
#define PIN_LED_2_PIO  PIOB
#define PIN_LED_2_ID   ID_PIOB
#define PIN_LED_2_TYPE PIO_OUTPUT_0
#define PIN_LED_2_ATTR PIO_DEFAULT

#define LED2_GPIO  (PIO_PB2_IDX)
#define LED2_FLAGS (PIO_OUTPUT_0 | PIO_DEFAULT)
#define LED2_ACTIVE_LEVEL 1

/** LED #3 pin definition. */
#define LED_3_NAME     "LED TRIGGER"
#define PIN_LED_3      {PIO_PB3, PIOB, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_LED_3_MASK PIO_PB3
#define PIN_LED_3_PIO  PIOB
#define PIN_LED_3_ID   ID_PIOB
#define PIN_LED_3_TYPE PIO_OUTPUT_0
#define PIN_LED_3_ATTR PIO_DEFAULT

#define LED3_GPIO  (PIO_PB3_IDX)
#define LED3_FLAGS (PIO_OUTPUT_0 | PIO_DEFAULT)
#define LED3_ACTIVE_LEVEL 1


#define BOARD_NUM_OF_LED 4

/** Push button #0 definition. Attributes = pull-up + debounce + interrupt on rising edge. */
#ifdef BOARD_REV_A
#define PUSHBUTTON_1_NAME    "SW1"
#endif

#ifdef BOARD_REV_B
#define PUSHBUTTON_1_NAME    "BP2"
#endif
#define GPIO_PUSH_BUTTON_1       (PIO_PA5_IDX)
#define GPIO_PUSH_BUTTON_1_FLAGS (PIO_INPUT | PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE)

#define PIN_PUSHBUTTON_1 {PIO_PA5, PIOA, ID_PIOA, PIO_INPUT, PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE}
#define PIN_PUSHBUTTON_1_MASK PIO_PA5
#define PIN_PUSHBUTTON_1_PIO  PIOA
#define PIN_PUSHBUTTON_1_ID   ID_PIOA
#define PIN_PUSHBUTTON_1_TYPE PIO_INPUT
#define PIN_PUSHBUTTON_1_ATTR PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE

/** List of all push button definitions. */
#define PINS_PUSHBUTTONS    PIN_PUSHBUTTON_1

#define PIN_TC0_TIOA0       (PIO_PA0_IDX)
#define PIN_TC0_TIOA0_MUX   (IOPORT_MODE_MUX_B)
#define PIN_TC0_TIOA0_FLAGS (PIO_PERIPH_B | PIO_DEFAULT)

#define PIN_TC0_TIOA1       (PIO_PA15_IDX)
#define PIN_TC0_TIOA1_MUX   (IOPORT_MODE_MUX_B)
#define PIN_TC0_TIOA1_FLAGS (PIO_PERIPH_B | PIO_DEFAULT)

#define PIN_TC0_TIOA1_PIO   PIOA
#define PIN_TC0_TIOA1_MASK  PIO_PA15
#define PIN_TC0_TIOA1_ID    ID_PIOA
#define PIN_TC0_TIOA1_TYPE  PIO_PERIPH_B
#define PIN_TC0_TIOA1_ATTR  PIO_DEFAULT

#define PIN_TC0_TIOA2       (PIO_PA26_IDX)
#define PIN_TC0_TIOA2_MUX   (IOPORT_MODE_MUX_B)
#define PIN_TC0_TIOA2_FLAGS (PIO_PERIPH_B | PIO_DEFAULT)

#define PIN_TC0_TIOA2_PIO   PIOA
#define PIN_TC0_TIOA2_MASK  PIO_PA26
#define PIN_TC0_TIOA2_ID    ID_PIOA
#define PIN_TC0_TIOA2_TYPE  PIO_INPUT
#define PIN_TC0_TIOA2_ATTR  PIO_DEFAULT

/** SPI MISO pin definition. */
#define SPI_MISO_GPIO  (PIO_PA12_IDX)
#define SPI_MISO_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)
/** SPI MOSI pin definition. */
#define SPI_MOSI_GPIO  (PIO_PA13_IDX)
#define SPI_MOSI_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)
/** SPI SPCK pin definition. */
#define SPI_SPCK_GPIO  (PIO_PA14_IDX)
#define SPI_SPCK_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)

/** SPI chip select 0 pin definition. (Only one configuration is possible) */
#define SPI_NPCS0_GPIO            (PIO_PA11_IDX)
#define SPI_NPCS0_FLAGS           (PIO_PERIPH_A | PIO_DEFAULT)


//! \name MXT143E Xplained top module
//@{
#define TARGET_TWI            TWI0
#define TARGET_XPLAINED_SPI   SPI
#define TARGET_CS             (PIO_PA11_IDX)
#define TARGET_SCK            (PIO_PA14_IDX)
#define TARGET_MOSI           (PIO_PA13_IDX)
#define TARGET_MISO           (PIO_PA12_IDX)
//@}


/** PCK1 (GOES TO ADC CLOCK) */
#define PIN_PCK1        (PIO_PA17_IDX)
#define PIN_PCK1_FLAGS  (PIO_PERIPH_B | PIO_DEFAULT)


#define PIN_PCK_1_MASK PIO_PA17
#define PIN_PCK_1_PIO  PIOA
#define PIN_PCK_1_ID   ID_PIOA
#define PIN_PCK_1_TYPE PIO_PERIPH_B
#define PIN_PCK_1_ATTR PIO_DEFAULT

/** PCK2 (GOES TO TARGET) */
#define TARGET_PCK      PIN_PCK2
#define PIN_PCK2        (PIO_PA18_IDX)
#define PIN_PCK2_FLAGS  (PIO_PERIPH_B | PIO_DEFAULT) ERROR CHECK THIS

#define PIN_PCK_2_MASK PIO_PA18
#define PIN_PCK_2_PIO  PIOA
#define PIN_PCK_2_ID   ID_PIOA
#define PIN_PCK_2_TYPE PIO_PERIPH_B  ERROR CHECK THIS
#define PIN_PCK_2_ATTR PIO_DEFAULT

/** TWI0 pins definition */
#define TWI0_DATA_GPIO   PIO_PA3_IDX
#define TWI0_DATA_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)
#define TWI0_CLK_GPIO    PIO_PA4_IDX
#define TWI0_CLK_FLAGS   (PIO_PERIPH_A | PIO_DEFAULT)


#define USART_TARGET USART1

/** USART1 pin RX */
#define PIN_USART1_RXD {PIO_PA21A_RXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_RXD_IDX   (PIO_PA21_IDX)
#define PIN_USART1_RXD_FLAGS (PIO_PERIPH_A | PIO_DEFAULT | PIO_PULLUP)
/** USART1 pin TX */
#define PIN_USART1_TXD {PIO_PA22A_TXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_TXD_IDX   (PIO_PA22_IDX)
#define PIN_USART1_TXD_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)

/* Parallel Data Capture (PDC) Module */
#define PIO_CAPTURE_CCLK_IDX           PIO_PA23_IDX
#define PIO_CAPTURE_EN1_IDX            PIO_PA15_IDX
#define PIO_CAPTURE_EN2_IDX            PIO_PA16_IDX
#define PIO_CAPTURE_EN2_MASK           PIO_PA16
#define PIO_CAPTURE_ENCTR_IDX          PIO_PA10_IDX
#define ADCCLK_EN_IDX				   PIO_PA19_IDX
#define ADCCLK_SRC_IDX				   PIO_PA20_IDX

#define PIO_CAPTURELATCH_nSET		   PIO_PA7_IDX
#define PIO_CAPTURELATCH_nRESET	       PIO_PA9_IDX


/* GPIO */
#define PIN_TARGET_nRST_IDX (PIO_PB14_IDX)
#define PIN_TARGET_PDIC_IDX (PIO_PA2_IDX)
#define PIN_TARGET_PDID_IDX (PIO_PA11_IDX)
#define PIN_TARGET_CLKOUT   (PIO_PA18_IDX)
#define PIN_TARGET_CLKOUT_FLAGS (PIO_PERIPH_B | PIO_DEFAULT)

#define PIN_GLITCH_IDX  (PIO_PA0_IDX)
#define PIN_GLITCH_MASK (PIO_PA0)

#define PIN_TARGET_GPIO1_IDX (PIO_PA21_IDX)
#define PIN_TARGET_GPIO2_IDX (PIO_PA22_IDX)
#define PIN_TARGET_GPIO3_IDX (PIO_PA8_IDX)
#define PIN_TARGET_GPIO4_IDX (PIO_PA1_IDX)
#define PIN_TARGET_GPIO4_MSK (PIO_PA1)

//! XMEGA PDI Programming Pins
#define PIN_PDIDTX_GPIO			  (PIO_PA6_IDX)
#define PIN_PDIDTX_OUT_FLAGS	  (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define PIN_PDIDTX_IN_FLAGS		  (PIO_TYPE_PIO_INPUT | PIO_DEFAULT)
#define PIN_PDIDTX_USART_FLAGS    (PIO_PERIPH_A | PIO_DEFAULT)

#define PIN_PDIDRX_GPIO			  (PIO_PA5_IDX)
#define PIN_PDIDRX_FLAGS		  (PIO_TYPE_PIO_INPUT | PIO_DEFAULT)
#define PIN_PDIDRX_USART_FLAGS    (PIO_PERIPH_A | PIO_DEFAULT)

#define PIN_PDIC_GPIO			  (PIO_PA2_IDX)
#define PIN_PDIC_OUT_FLAGS		  (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define PIN_PDIC_IN_FLAGS		  (PIO_TYPE_PIO_INPUT | PIO_DEFAULT)
#define PIN_PDIC_USART_FLAGS	  (PIO_PERIPH_B | PIO_DEFAULT)

#define USART_PDI                 USART0
#define USART_PDI_ID              ID_USART0

/*----------------------------------------------------------------------------*/
//#define CONSOLE_UART               UART0
//#define CONSOLE_UART_ID            ID_UART0

#define MPSSE_SCK_GPIO PIO_PA14_IDX
#define MPSSE_DOUT_GPIO PIO_PA13_IDX
#define MPSSE_DIN_GPIO PIO_PA12_IDX
#define MPSSE_TMS_GPIO PIO_PA11_IDX
#define MPSSE_GPIOL0 PIN_PDIC_GPIO

#endif // USER_BOARD_H
