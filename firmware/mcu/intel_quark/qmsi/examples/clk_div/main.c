/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Clock Divisor
 *
 * This app demonstrates changing the system clock speed.
 */

#include "clk.h"
#include "qm_uart.h"

/*
 * Helper macros to change oscillator settings and print out a string over UART
 * at 115200bps.
 */

#define SYSCLK_32MHZ_TO_UART_115200BPS (QM_UART_CFG_BAUD_DL_PACK(0, 17, 6))
#define SYSCLK_16MHZ_TO_UART_115200BPS (QM_UART_CFG_BAUD_DL_PACK(0, 8, 11))
#define SYSCLK_8MHZ_TO_UART_115200BPS (QM_UART_CFG_BAUD_DL_PACK(0, 4, 5))
#define SYSCLK_4MHZ_TO_UART_115200BPS (QM_UART_CFG_BAUD_DL_PACK(0, 2, 3))

#define switch_silicon_osc(freq, sysclk_div)                                   \
	do {                                                                   \
		clk_sys_set_mode(CLK_SYS_HYB_OSC_##freq,                       \
				 CLK_SYS_DIV_##sysclk_div);                    \
		{                                                              \
			uint32_t trim;                                         \
			clk_trim_read(&trim);                                  \
			QM_PRINTF("\tsource: silicon osc " #freq               \
				  " (trim code: 0x%x), divisor: " #sysclk_div  \
				  "\n",                                        \
				  (unsigned int)trim);                         \
		}                                                              \
	} while (0);

#define switch_xtal_osc(sysclk_div)                                            \
	do {                                                                   \
		clk_sys_set_mode(CLK_SYS_CRYSTAL_OSC,                          \
				 CLK_SYS_DIV_##sysclk_div);                    \
		QM_PUTS("\tsource: XTAL osc (32MHz), divisor: " #sysclk_div);  \
	} while (0);

int main(void)
{
	QM_PUTS("Starting: QMSI clock divisor example");

	QM_PUTS("\n32MHz sysclk:");
	stdout_uart_setup(SYSCLK_32MHZ_TO_UART_115200BPS);
	switch_silicon_osc(32MHZ, 1);
	switch_xtal_osc(1);

	QM_PUTS("\n16MHz sysclk:");
	stdout_uart_setup(SYSCLK_16MHZ_TO_UART_115200BPS);
	switch_silicon_osc(32MHZ, 2);
	switch_silicon_osc(16MHZ, 1);
	switch_xtal_osc(2);

	QM_PUTS("\n8MHz sysclk:");
	stdout_uart_setup(SYSCLK_8MHZ_TO_UART_115200BPS);
	switch_silicon_osc(32MHZ, 4);
	switch_silicon_osc(16MHZ, 2);
	switch_silicon_osc(8MHZ, 1);
	switch_xtal_osc(4);

	QM_PUTS("\n4MHz sysclk:");
	stdout_uart_setup(SYSCLK_4MHZ_TO_UART_115200BPS);
	switch_silicon_osc(32MHZ, 8);
	switch_silicon_osc(16MHZ, 4);
	switch_silicon_osc(8MHZ, 2);
	switch_silicon_osc(4MHZ, 1);
	switch_xtal_osc(8);

	QM_PUTS("Finished: QMSI clock divisor example");

	return 0;
}
