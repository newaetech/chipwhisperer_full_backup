#ifndef _LPC55S6X_HAL_H_
#define _LPC55S6X_HAL_H_

#include "fsl_common.h"
#include "fsl_reset.h"
#include "fsl_gpio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME "LPCXpresso5500"

/*! @brief The UART to use for debug messages. */
/* TODO: rename UART to USART */
#define BOARD_DEBUG_UART_TYPE kSerialPort_Uart
#define BOARD_DEBUG_UART_BASEADDR (uint32_t) USART0
#define BOARD_DEBUG_UART_INSTANCE 0U
#define BOARD_DEBUG_UART_CLK_FREQ 12000000U
#define BOARD_DEBUG_UART_CLK_ATTACH kFRO12M_to_FLEXCOMM0
#define BOARD_DEBUG_UART_RST kFC0_RST_SHIFT_RSTn
#define BOARD_DEBUG_UART_CLKSRC kCLOCK_Flexcomm0
#define BOARD_UART_IRQ_HANDLER FLEXCOMM0_IRQHandler
#define BOARD_UART_IRQ FLEXCOMM0_IRQn

#define BOARD_DEBUG_UART_TYPE_CORE1 kSerialPort_Uart
#define BOARD_DEBUG_UART_BASEADDR_CORE1 (uint32_t) USART1
#define BOARD_DEBUG_UART_INSTANCE_CORE1 1U
#define BOARD_DEBUG_UART_CLK_FREQ_CORE1 12000000U
#define BOARD_DEBUG_UART_CLK_ATTACH_CORE1 kFRO12M_to_FLEXCOMM1
#define BOARD_DEBUG_UART_RST_CORE1 kFC1_RST_SHIFT_RSTn
#define BOARD_DEBUG_UART_CLKSRC_CORE1 kCLOCK_Flexcomm1
#define BOARD_UART_IRQ_HANDLER_CORE1 FLEXCOMM1_IRQHandler
#define BOARD_UART_IRQ_CORE1 FLEXCOMM1_IRQn

#define BOARD_DEBUG_UART_BAUDRATE 38400U
#define BOARD_DEBUG_UART_BAUDRATE_CORE1 115200U

#define BOARD_XTAL0_CLK_HZ                         16000000U  /*!< Board xtal frequency in Hz */
#define BOARD_XTAL32K_CLK_HZ                          32768U  /*!< Board xtal32K frequency in Hz */

#define IOCON_PIO_DIGITAL_EN 0x0100u  /*!<@brief Enables digital function */
#define IOCON_PIO_FUNC1 0x01u         /*!<@brief Selects pin function 1 */
#define IOCON_PIO_INV_DI 0x00u        /*!<@brief Input function is not inverted */
#define IOCON_PIO_MODE_INACT 0x00u    /*!<@brief No addition pin function */
#define IOCON_PIO_OPENDRAIN_DI 0x00u  /*!<@brief Open drain is disabled */
#define IOCON_PIO_SLEW_STANDARD 0x00u /*!<@brief Standard mode, output slew rate control is enabled */

/*!
 * @brief This function executes default configuration of clocks.
 *
 */
void BOARD_InitBootClocks(void);
/*******************************************************************************
 ******************** Configuration BOARD_BootClockFRO12M **********************
 ******************************************************************************/
/*******************************************************************************
 * Definitions for BOARD_BootClockFRO12M configuration
 ******************************************************************************/
#define BOARD_BOOTCLOCKFRO12M_CORE_CLOCK           12000000U  /*!< Core clock frequency: 12000000Hz */
void BOARD_BootClockFRO12M(void);
/*******************************************************************************
 * Definitions for BOARD_BootClockFROHF96M configuration
 ******************************************************************************/
#define BOARD_BOOTCLOCKFROHF96M_CORE_CLOCK         96000000U  /*!< Core clock frequency: 96000000Hz */

void BOARD_InitDebugConsole(void);
void BOARD_InitDebugConsole_Core1(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void); /* Function assigned for the Cortex-M33 (Core #0) */

void init_uart(void);
void putch(char c);
char getch(void);

void trigger_setup(void);
void trigger_low(void);
void trigger_high(void);

uint32_t get_rand(void);

#endif /* _LPC55S6X_HAL_H_ */
