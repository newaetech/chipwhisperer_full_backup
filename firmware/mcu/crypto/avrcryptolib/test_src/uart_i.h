/* uart_i.h */
/*
    This file is part of the AVR-uart_ni.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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
/**
 * \file     uart_i.h
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte
 * \date     2009-07-24
 * \license  GPLv3 or later
 * \defgroup uart_i
 * \brief    declaration for interrupt based uart
 * \details
 * This implementation of the uart-interface of AVR microcontrollers uses the
 * interrup architecture and can be used to handle serial communication in the
 * background.
 * The uart is configured at compile-time by some special defines starting with
 * \a UART0_ for configuring the first uart and \a UART1_ for the second.
 * Some settings use symbolic values defined in uart_defs.h .
 * The following options are available:
 *   - \a *_I enables the interrupt based driver for this uart
 *      - \a 0 disables driver
 *      - \a 1 enables driver
 *   - \a *_BAUD_RATE sets the baudrate for this uart (value is the baudrate)
 *   - \a *_STOPBITS sets the amount of stop bits for this uart
 *      - \a UART_STOPBITS_1 for one stop bit
 *      - \a UART_STOPBITS_2 for two stop bits
 *   - \a *_DATABITS sets the amount of data bits for this uart
 *      - \a UART_DATABITS_5 for five data bits
 *      - \a UART_DATABITS_6 for six data bits
 *      - \a UART_DATABITS_7 for seven data bits
 *      - \a UART_DATABITS_8 for eight data bits
 *   - \a *_PARATY sets the mode for paraty calculation for this uart
 *      - \a UART_PARATY_NONE ignore paraty
 *      - \a UART_PARATY_ODD  odd paraty
 *      - \a UART_PARATY_EVEN even paraty
 *   - \a *_RXBUFFER_SIZE size of the recieve buffer in bytes
 *   - \a *_TXBUFFER_SIZE size of the transmitt buffer in bytes
 *   - \a *_SWFLOWCTRL enable/diasable software flow control (via XON & XOFF)
 *      - \a 0 disable software flow control
 *      - \a 1 enable software flow control
 *   - \a *_THRESH_HIGH set upper limit for the rx buffer, which causes an XOFF
 *                      to be send when crossed (only relevant if software flow
 *                                            control is enabled)
 *   - \a *_THRESH_LOW set lower limit for the rx buffer, which causes an XON to
 *                      be send when crossed and an XOFF has been send previously
 *                      (only relevant if software flow control is enabled)
 *   - \a *_HOOK enable/disable implementation of the hook feature
 *               (\ref uart0_sethook())
 *      - \a 0 disable hook feature
 *      - \a 1 enable hook feature
 *
 */

/*@{*/
#ifndef UART_I_H_
#define UART_I_H_

#include "config.h"
#include "circularbytebuffer.h"
#include <stdint.h>

/**
 * \brief storage type for uart0 context
 *
 * This type is used to store uart0 specific global variables.
 * It contains a pointer to the buffer instances and when neccessary
 * a pointer to the hook function and an indicator if the hook is
 * currently executed.
 * If software flow control is enabled it also contains flags for flow control.
 */
typedef struct{
	circularbytebuffer_t rxb; /**< recieve buffer */
	circularbytebuffer_t txb; /**< transmitt buffer*/
#if UART0_HOOK
	void(*hook)(uint8_t);     /**< pointer to the hook function */
	volatile uint8_t hook_running; /**< flag indicating if the hook is running */
#endif
#if UART0_SWFLOWCTRL
	volatile uint8_t txon;  /**< flag indicating if we are allowed to send data */
	volatile uint8_t rxon;  /**< flag indicating if we have send an \a XOFF */
#endif
} uart0_ctx_t;


/**
 * \brief storage type for uart1 context
 *
 * This type is used to store uart1 specific global variables.
 * It contains a pointer to the buffer instances and when neccessary
 * a pointer to the hook function and an indicator if the hook is
 * currently executed.
 * If software flow control is enabled it also contains flags for flow control.
 */
typedef struct{
	circularbytebuffer_t rxb; /**< recieve buffer */
	circularbytebuffer_t txb; /**< transmitt buffer */
#if UART1_HOOK
	void(*hook)(uint8_t);     /**< pointer to the hook function */
	volatile uint8_t hook_running; /**< flag indicating if the hook is running */
#endif
#if UART1_SWFLOWCTRL
	volatile uint8_t txon;  /**< flag indicating if we are allowed to send data */
	volatile uint8_t rxon;  /**< flag indicating if we have send an \a XOFF */
#endif
} uart1_ctx_t;

#if UART0_I

/**
 * \brief initialize uart0.
 *
 * This function initializes the first uart according to the parameter specified
 * in config.h .
 */
void uart0_init(void);

/**
 * \brief send data through uart0.
 *
 * This function sends data through the first uart
 * (the data size is debfined in config.h).
 * \param c data to send
 */
void uart0_putc(uint16_t c);

/**
 * \brief read data from uart0.
 *
 * This function reads data from the first uart
 * (the data size is debfined in config.h).
 * \return data recived by uart0
 */
uint16_t uart0_getc(void);

/**
 * \brief checks if data is available.
 *
 * This function checks the state of the input buffer of uart0 and
 * returns if data is available or not.
 * \return zero if no data is available else a value different from zero is returned
 */
uint8_t uart0_dataavail(void);

#if	UART0_HOOK
/**
 * \brief sets the hook for uart0.
 *
 * This function modifys the way the software handels incomming data.
 * When the hook is set to \a NULL (which is the default) incomming data is buffered
 * in a special ringbuffer and read by \ref uart0_getc(). If the hook is set to a
 * different value, this value is interpret as a function pointer. The hook (the
 * function where the function pointer points to) is called with the recieved data
 * as single parameter. Any value returned by the hook is discarded.

 * \note If the hook is set \ref uart0_getc() will not return, as the
 * ringbuffer is bypassed.
 * \param fpt pointer to thae handler function for recieved data
 */
void uart0_sethook(void(*fpt)(uint8_t));
#endif


#endif /* UART0_I */

#if UART1_I
/**
 * \brief initialize uart1.
 *
 * This function initializes the second uart according to the parameter specifyed
 * in config.h .
 */
void uart1_init(void);

/**
 * \brief send data through uart1.
 *
 * This function sends data through the second uart
 * (the data size is debfined in config.h).
 * \param c data to send
 */
void uart1_putc(uint16_t c);

/**
 * \brief read data from uart1.
 *
 * This function reads data from the second uart
 * (the data size is debfined in config.h).
 * \return data recived by uart1
 */
uint16_t uart1_getc(void);

/**
 * \brief checks if data is available.
 *
 * This function checks the state of the input buffer of uart1 and
 * returns if data is available or not.
 * \return zero if no data is available else a value different from zero is returned
 */
uint8_t uart1_dataavail(void);

/**
 * \brief sets the hook for uart1.
 *
 * This function modifys the way the software handels incomming data.
 * When the hook is set to \a NULL (which is the default) incomming data is buffered
 * in a special ringbuffer and read by \ref uart1_getc(). If the hook is set to a
 * different value, this value is interpret as a function pointer. The hook (the
 * function where the function pointer points to) is called with the recieved data
 * as single parameter. Any value returned by the hook is discarded.

 * \note If the hook is set \ref uart1_getc() will not return, as the
 * ringbuffer is bypassed.
 * \param fpt pointer to thae handler function for recieved data
 */
void uart1_sethook(void(*fpt)(uint8_t));
#endif

/*@}*/

#endif /* UART_I_H_ */
