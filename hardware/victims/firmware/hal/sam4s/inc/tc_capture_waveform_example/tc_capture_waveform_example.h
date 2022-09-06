/**
 * \file
 *
 * \brief TC Capture Waveform Example for SAM.
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

#ifndef TC_CAPTURE_WAVEFORM_EXAMPLE_H_INCLUDED
#define TC_CAPTURE_WAVEFORM_EXAMPLE_H_INCLUDED

/**
 * \page asfdoc_sam_drivers_tc_example Timer Counter Capture Waveform Example
 *
 * \section asfdoc_sam_drivers_tc_example_purpose Purpose
 *
 * This example indicates how to use the Timer Counter in capture mode and
 * waveform mode in order to measure the pulse frequency and count the total
 * pulse number of an external signal injected into the device's TIOA pin.
 *
 *
 * \section asfdoc_sam_drivers_tc_example_requirements Requirements
 *
 * This package can be used with SAM4 evaluation kits such as the SAM4L EK,
 * the SAM4L Xplained Pro, and other evaluation kits. Refer to the list of
 * kits available for the actual device on http://www.atmel.com.
 *
 * It generates a waveform on the Timer Counter pin PIN_TC_WAVEFORM,
 * and it captures a waveform from pin PIN_TC_CAPTURE. Refer to the
 * respective device evaluation kit's board file (conf_board.h) in order
 * to examine these #defines.
 *
 * Connect PIN_TC_WAVEFORM to PIN_TC_CAPTURE on the evaluation kit.
 *
 *
 * \section asfdoc_sam_drivers_tc_example_description Description
 *
 * This example shows how to configure the Timer Counter in both waveform and
 * capture operating modes. In capture mode, a pulse signal (output from PIN_TC_WAVEFORM)
 * is connected to PIN_TC_CAPTURE, and Register A (RA) and Register B (RB) will be loaded
 * when their programmed event occurs.
 *
 * When the Timer Counter interrupt occurs, its interrupt handler reads the RA
 * and RB register values (for computing pulse frequency) and also increases the total
 * pulse count number. The current pulse frequency and total pulse count number are
 * both output via the debug UART.
 *
 * The code can be roughly broken down as follows:
 * <ul>
 * <li>Select pre-defined waveform frequency and duty cycle to be generated
 * <li>Configure TC_CHANNEL_WAVEFORM as waveform output
 * <li>Configure TC_CHANNEL_CAPTURE as a capture input
 * <li>Configure capture Register A (RA) to be loaded when a rising edge on TIOA occurs
 * <li>Configure capture Register B (RB) to be loaded when a falling edge on TIOA occurs
 * <li>Configure a Timer Counter interrupt and enable the RB load interrupt
 * <li>Pressing 'c' in the terminal window, starts capture
 * <li>Pressing 's' in the terminal window, stops capture and dumps the information captured so far
 * </ul>
 *
 *
 * \section asfdoc_sam_drivers_tc_example_files Main Files
 *
 * - tc.c: Timer Counter driver
 * - tc.h: Timer Counter driver header file
 * - tc_capture_waveform_example.c: Timer Counter example application
 *
 *
 * \section asfdoc_sam_drivers_tc_example_compilinfo Compilation Information
 *
 * This software is written for GNU GCC and IAR Embedded Workbench&reg;
 * for Atmel&reg;. Other compilers may or may not work.
 *
 *
 * \section asfdoc_sam_drivers_tc_example_usage Usage
 *
 * -# Build the program and download it into the evaluation board.
 * -# On the computer, open, and configure a terminal application.
 *    (e.g., HyperTerminal on Microsoft&reg; Windows&reg;) with these settings:
 *   - 115200 baud
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# Start the application.
 * -# In the terminal window, the following text should appear:
 *    \code
 *     -- TC capture waveform example  xxx --
 *     -- xxxxxx-xx
 *     -- Compiled: xxx xx xxxx xx:xx:xx -- \endcode
 * -# Choose the item from the following menu to test:
 *    \code
 *     Menu :
 *     ------
 *       Output waveform property:
 *       0: Set Frequency =  178 Hz, Duty Cycle = 30%
 *       1: Set Frequency =  375 Hz, Duty Cycle = 50%
 *       2: Set Frequency =  800 Hz, Duty Cycle = 75%
 *       3: Set Frequency = 1000 Hz, Duty Cycle = 80%
 *       4: Set Frequency = 4000 Hz, Duty Cycle = 55%
 *       -------------------------------------------
 *       c: Capture waveform from TC(TC_PERIPHERAL) channel(TC_CHANNEL_CAPTURE)
 *       s: Stop capture and display captured information
 *       h: Display menu
 *     ------ \endcode
 */

#endif /* TC_CAPTURE_WAVEFORM_EXAMPLE_H_INCLUDED */
