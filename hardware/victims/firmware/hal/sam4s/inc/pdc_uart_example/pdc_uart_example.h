/**
 * \file
 *
 * \brief SAM Peripheral DMA Controller Example.
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef PDC_UART_EXAMPLE_H_INCLUDED
#define PDC_UART_EXAMPLE_H_INCLUDED

/**
 * \page asfdoc_sam_drivers_pdc_example Peripheral DMA Controller Example
 *
 * \section asfdoc_sam_drivers_pdc_example_purpose Purpose
 *
 * The pdc_uart example demonstrates how to use PDC driver to receive/send
 * data from/to the UART.
 *
 * \section asfdoc_sam_drivers_pdc_example_requirements Requirements
 *
 * This example can be used on any SAM3/4 boards.
 *
 * \section asfdoc_sam_drivers_pdc_example_description Description
 *
 * The SAM controller waits for BUFFER_SIZE data to receive from the UART.
 * As soon as the expected amount of data is received, the whole buffer is
 * sent back to the terminal.
 *
 * \section asfdoc_sam_drivers_pdc_example_usage Usage
 *
 * -# Build the program and download it into the evaluation board.
 * -# On the computer, open, and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft&reg; Windows&reg;) with these settings:
 *   - 115200 baud
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# In the terminal window, the following text should appear (values depend
 *    on the board and chip used):
     \verbatim
      -- PDC Uart Example xxx --
      -- xxxxxx-xx
      -- Compiled: xxx xx xxxx xx:xx:xx --
     \endverbatim
 * -# The sent text should appear.
 */

 #endif /* PDC_UART_EXAMPLE_H_INCLUDED */
 
